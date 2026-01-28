using System;
using System.Collections.Generic;
using System.Linq;
using ExcelDna.Integration;
using System.Collections;
using System.Diagnostics;
using System.Reflection;
using System.Security.Policy;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.Window;
using System.IO;



namespace FMTXLL
{

    public static class FMTCache
    {
        
        public static object data;
        public static List<string> tempfiles = new List<string>();
        public static object get(string MethodName, 
            object[] Arguments,object Orientation)
        {
            object Returned;
            if (!empty())
                {
                Returned = callFMTMethod(MethodName, Arguments);
            }else {
                Returned = -1.0;
            }
            return formatObjectToExcel(Returned, Orientation);
        }

        public static object callFMTMethod(string MethodName, object[] Arguments)
        {
            if (excelcachetype == null)
            {
                reportAndCrash("Aucun modèle chargé");
            }
            Type[] Types = new Type[] { };
            if (Arguments != null)
                {
                    Types = new Type[Arguments.Length];
                    for (int i = 0; i < Arguments.Length; ++i)
                    {
                        Types[i] = Arguments[i].GetType();
                        if (Arguments[i].GetType() == typeof(string))
                        {
                            Arguments[i] = Arguments[i].ToString().ToUpper();
                        }
                    }
                }
            var MethodInfo = SafeGetMethod(MethodName, Types);
            object Returned = MethodInfo.Invoke(data, Arguments);
            checkLibCrash();
            return Returned;
        }


        public static void reportAndCrash(string message,bool closeexcel=true,string windowname = "Erreur!")
        {
            System.Windows.Forms.Form window = new ErrorWindow(message, windowname);
            window.ShowDialog();
            if (closeexcel)
            {
                ExcelReference reference = (ExcelReference)XlCall.Excel(XlCall.xlfCaller);
                string internalSheetName = (string)XlCall.Excel(XlCall.xlSheetNm, reference);
                System.Text.RegularExpressions.Match match = System.Text.RegularExpressions.Regex.Match(internalSheetName, @"\[(.*)\](.*)");
                string workbookName = match.Groups[1].Value;
                Microsoft.Office.Interop.Excel.Application excelApp = (Microsoft.Office.Interop.Excel.Application)ExcelDna.Integration.ExcelDnaUtil.Application;
                excelApp.Workbooks[workbookName].Close();
                excelApp.Quit();
            }


        }

        public static bool empty()
        {
            return (wrapperassembly == null || excelcachetype == null || data == null);
        }

        public static void initialize(string pathtoFMTlib)
        {
            wrapperassembly = System.Reflection.Assembly.LoadFile(pathtoFMTlib);
            excelcachetype = wrapperassembly.GetType("Wrapper.FMTexcelcache");
            if (excelcachetype == null)//crash
            {
                reportAndCrash("No class named Wrapper.FMTexcelcache exist");
            }
            System.Reflection.ConstructorInfo constructor = excelcachetype.GetConstructor(Type.EmptyTypes);
            data = constructor.Invoke(null);
            if (data == null)//crash
            {
                reportAndCrash("Cannot create class Wrapper.FMTexcelcache");
            }

            gotexceptionmethod = SafeGetMethod("gotexception", new Type[] { });
           
            checkLibCrash();
        }
        
        public static void add(string primary, string scenario,bool printlog)
        {
            System.Reflection.MethodInfo methodInfo = SafeGetMethod("add", new Type[] { typeof(string), typeof(string) });
            object[] parameters = new object[2];
            parameters[0] = primary;
            parameters[1] = scenario;
            bool isright = (bool)methodInfo.Invoke(data, parameters);
            if (printlog||!isright)
                {
                string name = "Lecture de " + primary + "/" + scenario;
                checkLibCrash(isright, name);
            } else{//write out
                var getlogmethod = excelcachetype.GetMethod("getlogoutput");
                var outlog = getlogmethod.Invoke(data, null);
                string values = outlog.ToString();
                }
        }

        public static void addAndBuild(string primary, string scenario, int length,bool printlog)
        {
            System.Reflection.MethodInfo methodInfo = SafeGetMethod("addAndBuild", new Type[] { typeof(string), typeof(string), typeof(int) });
            object[] parameters = new object[3];
            parameters[0] = primary;
            parameters[1] = scenario;
            parameters[2] = length;
            bool isright = (bool)methodInfo.Invoke(data, parameters);
            if (printlog || !isright)
            {
                string name = "Lecture de " + primary + "/" + scenario;
                checkLibCrash(isright, name);
            }
            else
            {//write out
                var getlogmethod = excelcachetype.GetMethod("getlogoutput");
                var outlog = getlogmethod.Invoke(data, null);
                string values = outlog.ToString();
            }
        }
        public static void remove(string primary, string scenario)
        {
            get("remove", new object[] { primary, scenario },false);
        }
        public static object readnsolveTemplates(string primary, string folder, int length,bool printlog,bool solve,object Orientation)
        {
            List<string> scenarios = new List<string>();
            System.Reflection.MethodInfo methodInfo = SafeGetMethod("readnsolveTemplates", new Type[] { typeof(string), typeof(string), typeof(int), typeof(bool) });
            object[] parameters = new object[4];
            parameters[0] = primary;
            parameters[1] = folder;
            parameters[2] = length;
            parameters[3] = solve;
            scenarios = (List<string>)methodInfo.Invoke(data, parameters);
            if (printlog || !scenarios.Any())
            {
                string name = "Lecture du template " + primary + "/" + folder;
                checkLibCrash(scenarios.Any(), name);
            }
            else
            {//write out
                System.Reflection.MethodInfo getlogmethod = SafeGetMethod("getlogoutput", new Type[] {});
                var outlog = getlogmethod.Invoke(data, null);
                string values = outlog.ToString();
            }
            return formatObjectToExcel(scenarios,Orientation);
        }
       
        public static int size()
        {
            if (excelcachetype!=null)
            { 
                System.Reflection.MethodInfo methodInfo = SafeGetMethod("size", new Type[] {});
                var r = methodInfo.Invoke(data, null);
                return int.Parse(r.ToString());
            }
            return 0;
        }
       
        public static bool getThemeMap(string primaryname, string scenario,string jpeglocation, int themeid, List<string> attributes)
        {
            return (bool)get("writejpeg", new object[] { primaryname, scenario, jpeglocation, themeid, attributes }, false);
        }


        public static void clearTempFiles()
        {
            string folder = System.IO.Path.GetTempPath();
            foreach (string filepath in tempfiles)
            {
                string filename = System.IO.Path.GetFileNameWithoutExtension(filepath);
                System.IO.DirectoryInfo di = new System.IO.DirectoryInfo(folder);
                System.IO.FileInfo[] files = di.GetFiles(filename + ".*");
                foreach (System.IO.FileInfo file in files)
                {
                    file.Delete();
                }
            }
        }


        public static object getRotations(string PrimaryName, string Scenario,
            string ThemeSelection, string ActionAggregate, object Orientation)
        {
            object Returned = -1;
            if (!empty())
                {
                List<string> sortedRotations = (List<string>)callFMTMethod("getRotationsKeys", new object[] { PrimaryName, Scenario, ThemeSelection, ActionAggregate });
                Returned = formatObjectToExcel(sortedRotations, Orientation);
                }
            return Returned;
        }

        public static object getRotationMask(string PrimaryName, string Scenario,
            string Rotation, int ThemeId, object Orientation, string ActionAggregate)
        {
            object Returned = -1;
            if (!empty())
            {
                List<string> Attributes = new List<string>();
                foreach (string Attribute in (List<string>)callFMTMethod("getattributes", new object[] { PrimaryName, Scenario, ThemeId - 1, "?", false }))
                {
                    string ThemeSelection = "THEME" + ThemeId.ToString() + "=" + Attribute;
                    bool gotSubRotations = (bool)callFMTMethod("containsRotations", new object[] { PrimaryName, Scenario, Rotation, ThemeSelection, ActionAggregate });
                    if (gotSubRotations)
                    {
                        Attributes.Add(Attribute);
                    }
                }
                Returned = formatObjectToExcel(Attributes, Orientation);
            }
            return Returned;
        }

        public static object getMinimalRotation(string PrimaryName, string Scenario,
            string ThemeSelection, string Rotation, string ActionAggregate)
        {
            return getRotationBounds(PrimaryName, Scenario, ThemeSelection, Rotation, ActionAggregate)[0];
        }

        public static object getMaxmimalRotation(string PrimaryName, string Scenario,
            string ThemeSelection, string Rotation, string ActionAggregate)
        {
            return getRotationBounds(PrimaryName, Scenario, ThemeSelection, Rotation, ActionAggregate)[1];
        }
        private static System.Reflection.Assembly wrapperassembly;
        private static Type excelcachetype;
        private static System.Reflection.MethodInfo gotexceptionmethod;
        private static object[] getRotationBounds(string PrimaryName, string Scenario,
            string ThemeSelection, string Rotation,string ActionAggregate)
        {
            List<KeyValuePair<string, int>> AllRotations = (List<KeyValuePair<string,int>>)callFMTMethod("getRotations", new object[] { PrimaryName, Scenario, ThemeSelection, ActionAggregate });
            object[] Bounds = { Int32.MaxValue, 0 };
            int[] theRotations = AllRotations
                                       .Where(v => v.Key == Rotation)
                                       .Select(v => v.Value)
                                       .ToArray();
            if (theRotations.Length != 0)
                {
                foreach (int Value in theRotations)
                    {
                    if (Value < (int)Bounds[0])
                    {
                        Bounds[0] = Value;
                    }
                    if(Value > (int)Bounds[1])
                     {
                     Bounds[1] = Value;
                     }

                    }
            }else{
                Bounds[0] = "NA";
                Bounds[1] = "NA";
             }
            return Bounds;
        }

        private static void checkLibCrash(bool forceprint = false, string windowname = "Erreur!")
        {
            bool goterror = (bool)gotexceptionmethod.Invoke(data, null);
            if (forceprint || goterror)
            {
                var getlogmethod = excelcachetype.GetMethod("getlogoutput");
                var outlog = getlogmethod.Invoke(data, null);
                string values = outlog.ToString();
                if (!string.IsNullOrEmpty(values))
                {
                    reportAndCrash(values, goterror, windowname);
                }
            }
            if (goterror)
            {
                var unraise = excelcachetype.GetMethod("unraiseexception");
                unraise.Invoke(data, null);
            }
        }

        private static System.Reflection.MethodInfo SafeGetMethod(string methodname, Type[] parameters)
        {
            if (excelcachetype == null)//crash
            {
                reportAndCrash("No class named Wrapper.FMTexcelcache exist");
            }
            System.Reflection.MethodInfo methodInfo = excelcachetype.GetMethod(methodname, parameters);
            if (methodInfo == null)//crash
            {
                reportAndCrash("Cannot find " + methodname + " method in class Wrapper.FMTexcelcache");
            }
            return methodInfo;
        }

        

        static private object formatObjectToExcel(object BaseObject, object orientation)
        {
            if (BaseObject is IList &&
                BaseObject.GetType().IsGenericType &&
                BaseObject.GetType().GetGenericTypeDefinition().IsAssignableFrom(typeof(List<>)))
                {
                //List<T> strlist = (List<T>)BaseObject;
                //List<dynamic> strlist = (List<dynamic>)BaseObject;
                dynamic strlist = null;
                if (BaseObject.GetType() == typeof(List<int>))
                     {
                     strlist = (List<int>)BaseObject;
                }
                else if(BaseObject.GetType() == typeof(List<string>))
                {
                    strlist = (List<string>)BaseObject;

                }
                else if (BaseObject.GetType() == typeof(List<double>))
                {
                    strlist = (List<double>)BaseObject;

                }


                if (strlist.Count == 0)
                {
                    return ExcelEmpty.Value;
                }
                bool col = Optional.Check(orientation, true);
                int sizeoflist = strlist.Count;
                int rowsize = 1;
                int colsize = 1;
                if (col)
                {
                    rowsize = sizeoflist;
                }
                else
                {
                    colsize = sizeoflist;
                }
                object[,] result = new object[rowsize, colsize];
                int rowid = 0;
                int colid = 0;
                foreach (dynamic value in strlist)
                {
                    result[rowid, colid] = value;
                    if (col)
                    {
                        ++rowid;
                    }
                    else
                    {
                        ++colid;
                    }

                }
                return result;
            }else
            {
                return BaseObject;
            }
            
        }

    }


}
