using System;
using ExcelDna.Integration;
using ExcelDna.IntelliSense;
using System.Collections.Generic;


namespace FMTXLL
{

    
    public class FMT : IExcelAddIn
    {
        public static void appendToFormulaValue(string value)
        {
            Microsoft.Office.Interop.Excel.Application app = (Microsoft.Office.Interop.Excel.Application)ExcelDna.Integration.ExcelDnaUtil.Application;
            Microsoft.Office.Interop.Excel.Range range = app.ActiveCell;
            if (range != null)
            {
                if (value.Contains("="))
                {
                    range.Formula = value;
                }else
                    {
                    range.Formula += value;
                }
                
            }
            
        }
        private static string GetUniquejpegLocation()
        {
            string[] paths = { System.IO.Path.GetTempPath(), "Temp"+GetCallingCellReference() + ".bmp"};
            return System.IO.Path.Combine(paths);
        }
        private static string GetCallingCellReference()
            {
            ExcelReference reference = (ExcelReference)XlCall.Excel(XlCall.xlfCaller);
            string cellReference = (string)XlCall.Excel(XlCall.xlfAddress, 1 + reference.RowFirst,1 + reference.ColumnFirst);
            string sheetName = (string)XlCall.Excel(XlCall.xlSheetNm, reference);
            cellReference = sheetName.Substring(1, sheetName.IndexOf(".")-1) + cellReference.Replace("$","");
            cellReference = cellReference.Replace(" ", "");
            cellReference = cellReference.Replace("(", "");
            cellReference = cellReference.Replace(")", "");
            return cellReference;
            }
        private static void Getjpeg(string jpeglocation)
        {
            ExcelReference reference = (ExcelReference)XlCall.Excel(XlCall.xlfCaller);
            string internalSheetName = (string)XlCall.Excel(XlCall.xlSheetNm, reference);
            System.Text.RegularExpressions.Match match =System.Text.RegularExpressions.Regex.Match(internalSheetName, @"\[(.*)\](.*)");
            string workbookName = match.Groups[1].Value;
            string sheetName = match.Groups[2].Value;
            Microsoft.Office.Interop.Excel.Application excelApp = (Microsoft.Office.Interop.Excel.Application)ExcelDna.Integration.ExcelDnaUtil.Application;
            Microsoft.Office.Interop.Excel.Worksheet sheet = excelApp.Worksheets[sheetName];
            Microsoft.Office.Interop.Excel.Range oRange = (Microsoft.Office.Interop.Excel.Range)sheet.Cells[reference.RowFirst+2, reference.ColumnFirst+1];
            float Left = (float)((double)oRange.Left);
            float Top = (float)((double)oRange.Top);
            foreach(Microsoft.Office.Interop.Excel.Shape theshape in sheet.Shapes)
                {
                if (excelApp.Intersect(theshape.TopLeftCell, oRange)!=null)
                     {
                    theshape.Delete();
                     }
                }
            const float ImageSize = 500;
            sheet.Shapes.AddPicture(jpeglocation,Microsoft.Office.Core.MsoTriState.msoFalse, Microsoft.Office.Core.MsoTriState.msoCTrue, Left, Top, ImageSize, ImageSize);
        }

        static private List<string> StrObjectTolist(object values)
        {
            List<string> elements = new List<string>();
            if (values is Array)
            {
                if (((Array)values).Rank == 2)
                {
                    int columnCount = ((Array)values).GetLength(1);
                    int rowCount = ((Array)values).GetLength(0);
                    for (int i = 0; i < rowCount; i++)
                    {
                        for (int j = 0; j < columnCount; j++)
                        {
                            elements.Add((((Array)values).GetValue(i, j)).ToString().ToUpper());
                        }
                     }
                }
            }
            return elements;
        }


        
        public void AutoOpen()
        {
            //
           // string test = System.AppContext.BaseDirectory;//System.Reflection.Assembly.GetCallingAssembly().Location;//System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
           // FMTCache.reportAndCrash(test);
            string fmtlocation = System.AppContext.BaseDirectory;//System.Environment.GetEnvironmentVariable("FMT_LIB");
            string excelwrapperdll = System.IO.Path.Combine(fmtlocation, "FMTExcelWrapper.dll");
            if (string.IsNullOrEmpty(fmtlocation))
                {
                FMTCache.reportAndCrash("La librairie FMT_LIB ne se trouve pas dans les variables d'environnement");
                }else if(!System.IO.File.Exists(excelwrapperdll))
                    {
                    FMTCache.reportAndCrash("FMTExcelWrapper.dll n'existe pas dans "+ fmtlocation);
                    }else{
                        FMTCache.initialize(excelwrapperdll);
                        }
            IntelliSenseServer.Install();
            ExcelIntegration.RegisterUnhandledExceptionHandler(
            ex => "!!! EXCEPTION: " + ex.ToString());

        }

        public void AutoClose()
        {
            IntelliSenseServer.Uninstall();
        }
        
        private static object asyncValeurOutput(ref string primaryname,ref string scenario,ref string outputname,ref string themeselection,ref int period)
        {
                return FMTCache.get("getvalue", new object []{ primaryname, scenario, outputname, themeselection, period }, false);

        }
        

        [ExcelFunction(Description = "Obtenir la valeur d'un output", Category = "FMT", HelpTopic="Renvoie la valeur de l'output dans une cellule", IsMacroType = false,IsThreadSafe=true)]
        public static object VALEUROUTPUT(
            [ExcelArgument(Name = "Nom de l'output", Description = "Nom de l'output du modèle ex:(OVOLTOTREC)")]
            string outputname,
            [ExcelArgument(Name = "Sélection", Description = "Filtre de sélection ex:(THEME1=GS12 ou Groupe de strate=GS15 ou THEME1=GS12;THEME2=U2 ou THEME1={GS12,GS14}")]
            string themeselection,
            [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
            [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
            [ExcelArgument(Name = "Période", Description = "Période cible de l'output")]
            int period)
        {
            return asyncValeurOutput(ref primaryname, ref scenario, ref outputname, ref themeselection,ref period);
        }
        private static object asyncValeurCourbe(ref string yieldname, ref string themeselection,ref  string primaryname, ref string scenario, ref int period, ref int age)
        {

                return FMTCache.get("getyield", new object[] {primaryname,scenario,yieldname, themeselection,period,age },false);

        }

       [ExcelFunction(Description = "Obtenir la valeur d'un yield", Category = "FMT", HelpTopic = "Renvoie la valeur d'un yield dans une cellule", IsMacroType = false, IsThreadSafe = true)] 
        public static object VALEURCOURBE(
            [ExcelArgument(Name = "Nom du yield", Description = "Nom de du yield du modèle ex:(youvert)")]
            string yieldname,
            [ExcelArgument(Name = "Sélection", Description = "Filtre de sélection ex:(THEME1=GS12 ou Groupe de strate=GS15 ou THEME1=GS12;THEME2=U2 ou THEME1={GS12,GS14}")]
            string themeselection,
            [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
            [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
            [ExcelArgument(Name = "Période", Description = "Période cible")]
            int period,
             [ExcelArgument(Name = "Âge", Description = "Âge cible")]
            int age)
        {
            return asyncValeurCourbe(ref yieldname,ref themeselection,ref primaryname,ref scenario,ref period,ref age);

        }
        [ExcelFunction(Description = "Obtenir tous les attributs d'un thème et de sa valeur (colonnes ou lignes)", Category = "FMT", HelpTopic = "Renvoie les attributs d'un thème dans une colonne ou une ligne")]
        public static object NOMATTRIBUTS(
            [ExcelArgument(Name = "Identifiant du thème", Description = "Identifiant du thème ex:(3)")]
            int themeid,
            [ExcelArgument(Name = "Valeur du thème", Description = "Valeur du thème ex:(?,GS40,FC6005)")]
            string value,
            [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
            [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
            [ExcelArgument(Name = "Garder aggrégats", Description = "Si VRAI, retourne les noms dees aggrégats de les décomposer par attribut")]
            object gaggregates,
            [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {

                bool aggregates = Optional.Check(gaggregates, false);
                return FMTCache.get("getattributes", new object[] {primaryname, scenario, themeid - 1,value, aggregates }, orientation);

        }
        [ExcelFunction(Description = "Obtenir la description des attributs d'un thème et de sa valeur (colonnes ou lignes)", Category = "FMT", HelpTopic = "Renvoie les descriptions d'attributs d'un thème dans une colonne ou une ligne")]
        public static object DESCRIPTIONATTRIBUTS(
           [ExcelArgument(Name = "Identifiant du thème", Description = "Identifiant du thème ex:(3)")]
            int themeid,
           [ExcelArgument(Name = "Valeur du thème", Description = "Valeur du thème ex:(?,GS40,FC6005)")]
            string value,
           [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
           [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
           [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {

                return FMTCache.get("getattributesdescription", new object[] { primaryname, scenario, themeid - 1, value }, orientation);


        }
        [ExcelFunction(Description = "Obtenir tous les aggrégats d'un thème (colonne)", Category = "FMT", HelpTopic ="Renvoie tous les aggrégats d'un thème dans une colonne ou une ligne")]
        public static object NOMAGGREGATES(
            [ExcelArgument(Name = "Identifiant du thème", Description = "Identifiant du thème ex:(3)")]
            int themeid,
            [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
            [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
            [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {

                return FMTCache.get("getaggregates", new object[] { primaryname, scenario, themeid - 1 }, orientation);


        }
        [ExcelFunction(Description = "Obtenir tous les noms d'actions (colonne) d'un aggrégat d'action", Category = "FMT", HelpTopic = "Renvoie tous les noms d'actions d'un aggrégat dans une colonne ou une ligne")]
        public static object NOMACTIONS(
           [ExcelArgument(Name = "Aggregat d'action ou ?", Description = "Aggregat de l'action visée ex:(?)")]
            string actionaggregate,
          [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
          [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
           [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {
                return FMTCache.get("getactions", new object[] { primaryname, scenario, actionaggregate }, orientation);

        }
        [ExcelFunction(Description = "Obtenir toutes les séries", Category = "FMT", HelpTopic = "Renvoie toutes les séries d'une sélection de thème et d'aggrégat")]
        public static object ENCHAINEMENT(
         [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
         [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
         [ExcelArgument(Name = "Sélection", Description = "Filtre de sélection ex:(THEME1=GS12 ou Groupe de strate=GS15 ou THEME1=GS12;THEME2=U2 ou THEME1={GS12,GS14}")]
            string themeselection,
         [ExcelArgument(Name = "Aggregat d'action", Description = "Aggregat de l'action visée ex:(REGAFIN)")]
            string ActionAggregate,
         [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {

            return FMTCache.getRotations(primaryname, scenario, themeselection, ActionAggregate, orientation);

        }

        [ExcelFunction(Description = "Obtenir le temps de rotation minimale d'une série", Category = "FMT", HelpTopic = "Renvoie le temps de rotation minimale d'une série")]
        public static object ROTATIONMINIMALE(
        [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
        [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
        [ExcelArgument(Name = "Sélection", Description = "Filtre de sélection ex:(THEME1=GS12 ou Groupe de strate=GS15 ou THEME1=GS12;THEME2=U2 ou THEME1={GS12,GS14}")]
            string themeselection,
        [ExcelArgument(Name = "Série", Description = "Série sous le format ACT-APL-ACPIL")]
            string Serie,
         [ExcelArgument(Name = "Aggregat d'action", Description = "Aggregat de l'action visée ex:(REGAFIN)")]
            string ActionAggregate,
        [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {
            return FMTCache.getMinimalRotation(primaryname, scenario, themeselection,Serie, ActionAggregate);
        }

        [ExcelFunction(Description = "Obtenir le temps de rotation maximale d'une série", Category = "FMT", HelpTopic = "Renvoie le temps de rotation maximale d'une série")]
        public static object ROTATIONMAXIMALE(
        [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
        [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
        [ExcelArgument(Name = "Sélection", Description = "Filtre de sélection ex:(THEME1=GS12 ou Groupe de strate=GS15 ou THEME1=GS12;THEME2=U2 ou THEME1={GS12,GS14}")]
            string themeselection,
        [ExcelArgument(Name = "Série", Description = "Série sous le format ACT-APL-ACPIL")]
            string Serie,
         [ExcelArgument(Name = "Aggregat d'action", Description = "Aggregat de l'action visée ex:(REGAFIN)")]
            string ActionAggregate,
        [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {
            return FMTCache.getMaxmimalRotation(primaryname, scenario, themeselection, Serie, ActionAggregate);
        }

        [ExcelFunction(Description = "Obtenir les attributs ou la série est réalisée", Category = "FMT", HelpTopic = "Obtenir les attributs ou la série est réalisée")]
        public static object INFOENCHAINEMENT(
        [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
        [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
        [ExcelArgument(Name = "Série", Description = "Série sous le format ACT-APL-ACPIL")]
            string Serie,
        [ExcelArgument(Name = "Identifiant du thème", Description = "Identifiant du thème ex:(3)")]
            int themeid,
         [ExcelArgument(Name = "Aggregat d'action", Description = "Aggregat de l'action visée ex:(REGAFIN)")]
            string ActionAggregate,
        [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object Orientation)
        {
            return FMTCache.getRotationMask(primaryname, scenario,Serie, themeid,Orientation, ActionAggregate);
        }

        [ExcelFunction(Description = "Obtenir les développements sans actions", Category = "FMT", HelpTopic = "Renvoie tous les développements qui n'ont pas d'actions réalisable")]
        public static object NONTRAITE(
         [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
         [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
         [ExcelArgument(Name = "Sélection", Description = "Filtre de sélection ex:(THEME1=GS12 ou Groupe de strate=GS15 ou THEME1=GS12;THEME2=U2 ou THEME1={GS12,GS14}")]
            string themeselection,
         [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {

                return FMTCache.get("getnochoice", new object[] { primaryname, scenario, themeselection }, orientation);


        }

        [ExcelFunction(Description = "Obtenir tous les aggrégats d'une action", Category = "FMT", HelpTopic = "Renvoie tous les noms d'aggrégat d'une action")]
        public static object NOMACTIONAGGREGATES(
          [ExcelArgument(Name = "Nom de l'action", Description = "Nom de l'action ex:(act)")]
            string actionname,
         [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
         [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
          [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {

                return FMTCache.get("getactionaggregates", new object[] { primaryname, scenario, actionname }, orientation);


        }
        [ExcelFunction(Description = "Obtenir tous les noms d'outputs (colonne)", Category = "FMT", HelpTopic = "Renvoie tous les noms d'outputs dans une colonne ou une ligne")]
        public static object NOMOUTPUTS(
           [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
           [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
            [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {

                return FMTCache.get("getoutputs", new object[] { primaryname, scenario}, orientation);


        }
        [ExcelFunction(Description = "Obtenir tous les noms de yields (colonne)", Category = "FMT", HelpTopic = "Renvoie tous les noms de yields dans une colonne ou une ligne")]
        public static object NOMCOURBES(
           [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
           [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
            [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {

            return FMTCache.get("getyields", new object[] { primaryname, scenario }, orientation);


        }
        [ExcelFunction(Description = "Obtenir tous les noms de thèmes", Category = "FMT", HelpTopic = "Renvoie tous les noms de thèmes dans une colonne ou une ligne")]
        public static object NOMTHEMES(
          [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
          [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
            [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {

            return FMTCache.get("getyields", new object[] { primaryname, scenario }, orientation);


        }
        [ExcelFunction(Description = "Obtenir tous les scénarios d'un fichier primaire (colonne)", Category = "FMT", HelpTopic = "Renvoie tous les noms de scénario du fichier primaire dans une colonne ou une ligne")]
        public static object NOMSCENARIOS(
            [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
            [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {

            return FMTCache.get("getscenarios", new object[] { primaryname }, orientation);

        }
        [ExcelFunction(Description = "Obtenir tous les fichiers primaire (colonne)", Category = "FMT", HelpTopic = "Renvoie tous les noms de fichier primaire dans une colonne ou une ligne")]
        public static object NOMPRIMAIRES(
            [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {

            return FMTCache.get("getprimaries", null, orientation);


        }
        [ExcelFunction(Description = "Ajouter un scenario en cache. Si le modèle a été modifier, retirer le pour l'ajouter par la suite.", Category = "FMT", HelpTopic = "Renvoie rien mais ajoute un scénario dans la cache")]
        public static void AJOUTER(
           [ExcelArgument(Name = "Localisation du fichier primaire", Description = "Nom du fichier primaire ex:(D:/PC_14.pri)")]
             string primaryname,
           [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
             string scenarioname,
           [ExcelArgument(Name = "Sortie de lecture", Description = "Écrire la lecture du modèle (VRAI ou FAUX)")]
           object orientation)
        {
            if (!FMTCache.empty() && !(string.IsNullOrEmpty(primaryname) || string.IsNullOrEmpty(scenarioname)))
            {
                bool printlog = Optional.Check(orientation, true);
                FMTCache.add(primaryname, scenarioname, printlog);
                FMTRibbon.Refresh();
            }
        }
        [ExcelFunction(Description = "Retirer un scenario de la cache", Category = "FMT", HelpTopic = "Renvoie rien mais retire un scénario de la cache")]
        public static void RETIRER(
           [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
             string primaryname,
           [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
             string scenarioname)
        {
            if (!FMTCache.empty())
            {
                FMTCache.remove(primaryname, scenarioname);
                FMTRibbon.Refresh();
            }
        }

        [ExcelFunction(Description = "Ajoute et solutionne tous les modeles présents dans le dossier modèles à partir du fichier primaire", Category = "FMT", HelpTopic = "Renvoie la liste des modèles réalisable et ajoute les scénarios générés dans la cache")]
        public static object MODELES(
           [ExcelArgument(Name = "Localisation du fichier primaire", Description = "Nom du fichier primaire ex:(D:/PC_14.pri)")]
             string primaryname,
           [ExcelArgument(Name = "Dossier modèles", Description = "Dossier des modèles sous la forme D:/templates")]
             string folder,
           [ExcelArgument(Name = "Horizon de planification", Description = "Nombre de périodes d'optimisation")]
             int length,
           [ExcelArgument(Name = "Solutionner le test", Description = "Solutionner les modèles? (VRAI ou FAUX)")]
           object solve,
           [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
           object orientation)
        {
            object elements = new List<string>();
            if (!FMTCache.empty() && !(string.IsNullOrEmpty(primaryname) || string.IsNullOrEmpty(folder)))
            {
                bool solve_model = Optional.Check(solve, true);
                elements = FMTCache.readnsolveTemplates(primaryname, folder, length, true, solve_model, orientation);
                FMTRibbon.Refresh();
            }
            return elements;
        }

        [ExcelFunction(Description = "Obtenir les périodes du scénario", Category = "FMT", HelpTopic = "Renvoie tous les périodes du scénario dans une colonne ou une ligne")]
        public static object PERIODES(
           [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
             string primaryname,
           [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
             string scenarioname,
            [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {

            return FMTCache.get("getperiods", new object[] { primaryname, scenarioname }, orientation);
        }
        [ExcelFunction(Description = "Rempli un thème sur la carte et renvoi son contenu", Category = "FMT", HelpTopic = "Renvoie une carte en format jpeg du thème")]
        public static void PHOTOTHEME(
           [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
             string primaryname,
           [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
             string scenarioname,
           [ExcelArgument(Name = "Identifiant du thème", Description = "Identifiant du thème 1 à 20")]
             int themeid,
           [ExcelArgument(Name = "Attributs", Description = "Liste d'attributs pour colorer la carte")]
             object Attributs
            )
        {
            if (!FMTCache.empty())
            {
                string newjpeg = GetUniquejpegLocation();
                List<string> elements = StrObjectTolist(Attributs);
                if (elements.Count!=0)
                    {
                    if (FMTCache.getThemeMap(primaryname, scenarioname, newjpeg, themeid - 1, elements))
                        {
                        FMTCache.tempfiles.Add(newjpeg);
                        Getjpeg(newjpeg);
                        }
                    }
            }
        }
        [ExcelFunction(Description = "Renvoi la conformité des COS par rapport à leurs juxtaposition", Category = "FMT", HelpTopic = "Renvoi une liste de conformité par période")]
        public static object JUXTAPOSITION(
          [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
             string primaryname,
          [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
             string scenarioname,
          [ExcelArgument(Name = "Nom du yield", Description = "Yield d'ouverture et de fermeture du COS à utiliser")]
              string yieldname,
          [ExcelArgument(Name = "Output de ratio", Description = "Output de ratio de perturbation du COS entre (0 et 1)")]
              string output,
          [ExcelArgument(Name = "Ratio maximal", Description = "Raio maximal avant de considérer le COS perturbé")]
              double ratio,
          [ExcelArgument(Name = "Périmètre de voisinage", Description = "Ratio du périmètre que doivent partager les COS pour êtres voisins")]
              double perimetre,
          [ExcelArgument(Name = "Liste de Masque à valider", Description = "Liste de masques pour lesquels on veut valider la juxtaposition")]
             object masks,
          [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation
           )
        {
            if (!FMTCache.empty())
            {
                List<string> elements = StrObjectTolist(masks);
                if (elements.Count != 0)
                    {
                    return FMTCache.get("Juxtaposition", new object[] { primaryname, scenarioname, elements, yieldname, output, ratio, perimetre }, orientation);
                    }
            }
            return -1;
        }
        [ExcelFunction(Description = "Obtenir tous les avertissements d'une exception", Category = "FMT", HelpTopic = "Renvoie les avertissements de la construction d'un scénario dans une colonne ou une ligne")]
        public static object AVERTISSEMENT(
            [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
            [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
             [ExcelArgument(Name = "Identifiant de l'avertissement", Description = "Identifiant de l'avertissement ex:(5)")]
            int id,
            [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {
            return FMTCache.get("getbuildexceptions", new object[] { primaryname, scenario, id }, orientation);

        }

        [ExcelFunction(Description = "Obtenir tous les contraintes", Category = "FMT", HelpTopic = "Renvoie tous les noms de contraintes contenant un output donné dans une colonne ou une ligne")]
        public static object TROUVECONTRAINTES(
           [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
           [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
           [ExcelArgument(Name = "Nom de l'output", Description = "Nom de l'output ex:(OVOLTOTREC)")]
            string outputname,
            [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {
            return FMTCache.get("getconstraints", new object[] { primaryname, scenario, outputname }, orientation);

        }
        [ExcelFunction(Description = "Obtenir les statistiques du graph du modèle", Category = "FMT", HelpTopic = "Renvoie une liste de stratistique du graph: nombre de colonnes, lignes, branches, noeuds , lignes de transfert de superficie, lignes d'outputs et colonnes d'outputs")]
        public static object STATISTIQUESDUGRAPHS(
           [ExcelArgument(Name = "Nom du fichier primaire", Description = "Nom du fichier primaire ex:(PC_14)")]
            string primaryname,
           [ExcelArgument(Name = "Nom du scénario", Description = "Nom du scénario ex:(scenario_1)")]
            string scenario,
           [ExcelArgument(Name = "Sélection", Description = "Filtre de sélection ex:(THEME1=GS12 ou Groupe de strate=GS15 ou THEME1=GS12;THEME2=U2 ou THEME1={GS12,GS14}")]
            object themeselection,
            [ExcelArgument(Name = "Orientation", Description = "Format de retour (VRAI = plusieurs lignes, FAUX = plusieurs colonnes)")]
            object orientation)
        {
            String MASK_SUBSET = Optional.Check(themeselection, "");
            if (MASK_SUBSET.Length>0)
            {
                return FMTCache.get("getGraphStatsSubset", new object[] { primaryname, scenario,themeselection}, orientation);
            } 
            return FMTCache.get("getgraphstats", new object[] { primaryname, scenario }, orientation);
        }
        
    }
}

