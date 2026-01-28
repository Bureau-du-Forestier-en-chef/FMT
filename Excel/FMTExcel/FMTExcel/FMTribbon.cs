using System.Runtime.InteropServices;
using System.Windows.Forms;
using ExcelDna.Integration.CustomUI;
using System.Collections.Generic;
using System;
using System.Linq;
using System.Text;
using System.Drawing;

[System.ComponentModel.LookupBindingProperties]
public class CheckedListBox : System.Windows.Forms.CheckedListBox { };

namespace FMTXLL
{
    [ComVisible(true)]
    public class FMTRibbon : ExcelRibbon
    {
        private static IRibbonUI _ribbonUi;
        private string GetPath()
        {
            var filePath = string.Empty;
            using (System.Windows.Forms.OpenFileDialog openFileDialog = new System.Windows.Forms.OpenFileDialog())
            {
                openFileDialog.InitialDirectory = "c:\\";
                openFileDialog.Filter = "Fichier primaire (*.pri)|*.pri";
                openFileDialog.FilterIndex = 2;
                openFileDialog.RestoreDirectory = true;

                if (openFileDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    //Get the path of specified file
                    filePath = openFileDialog.FileName;
                }
            }
            return filePath;
        }
        private List<string> FindScenarios(ref String p_PrimaryFile,String Extension= "._seq")
        {
            string primaryfile = GetPath();
            p_PrimaryFile = primaryfile;
            string directory = System.IO.Path.GetDirectoryName(primaryfile);
            string filename = System.IO.Path.GetFileNameWithoutExtension(primaryfile);
            string[] paths = { directory, "Scenarios" };
            string pathtscenarios = System.IO.Path.Combine(paths);
            List<string> goodscenarios = new List<string>();
            
            if (System.IO.Directory.Exists(pathtscenarios))
            {
                string[] allscenarios = System.IO.Directory.GetDirectories(pathtscenarios);
                foreach (string scenario in allscenarios)
                {
                    string[] namelist = { scenario, filename + Extension };
                    string pathtoschedule = System.IO.Path.Combine(namelist);
                    if (System.IO.File.Exists(pathtoschedule))
                    {
                        string dirName = new System.IO.DirectoryInfo(scenario).Name;
                        goodscenarios.Add(dirName);
                    }
                }
            }
            //Now do the root
            Extension = Extension.Replace("_", string.Empty);
            string[] rootpath = { directory, filename + Extension };
            string test = System.IO.Path.Combine(rootpath);
            if (System.IO.File.Exists(System.IO.Path.Combine(rootpath)))
            {
                goodscenarios.Add("ROOT");
            }
        return goodscenarios;
        }
        public void addModel(IRibbonControl control1)
        {
            string PRIMARY_FILE = "";
            List<string> GOOD_SCENARIOS = FindScenarios(ref PRIMARY_FILE);
            ScenarioSelector selection = new ScenarioSelector(GOOD_SCENARIOS, "Sélection des scénarios", "Importer");
            foreach (string scenario in selection.getScenarios())
                {
                FMTCache.add(PRIMARY_FILE, scenario,true);
                }
            Refresh();
        }

        public void addUnsolvedModel(IRibbonControl control1)
        {
            string PRIMARY_FILE = "";
            List<string> GOOD_SCENARIOS = FindScenarios(ref PRIMARY_FILE,"._opt");
            ScenarioSelector selection = new ScenarioSelector(GOOD_SCENARIOS, "Sélection des scénarios", "Importer",true);
            List<string> SCENARIOS = selection.getScenarios();
            List<int> LENGTH = selection.getScenariosLength();
            var ZIPPED = LENGTH.Zip(SCENARIOS, (n, w) => new { Length = n, Scenario = w });
            foreach (var TO_ADD in ZIPPED)
            {
                FMTCache.addAndBuild(PRIMARY_FILE, TO_ADD.Scenario, TO_ADD.Length,true);
            }
            Refresh();
        }

        public void Ribbon_Load(IRibbonUI sender)
        {
            _ribbonUi = sender;
        }
        public bool removeModel_GetEnabled(IRibbonControl control)
        {
            return (FMTCache.size() > 0);
        }

        public bool addModel_GetEnabled(IRibbonControl control)
        {
            return (!FMTCache.empty());
        }

        public void removeModel(IRibbonControl control1)
            {
            List<string> primaries = (List<string>)FMTCache.callFMTMethod("getprimaries", null);
            ScenarioSelector primaryselection = new ScenarioSelector(primaries, "Sélection des fichiers primaires", "Explorer");
            foreach (string primary in primaryselection.getScenarios())
                {
                List<string> scenarios = (List<string>)FMTCache.callFMTMethod("getscenarios", new object[] { primary });
                ScenarioSelector scenarioselection = new ScenarioSelector(scenarios, "Sélection des scénarios", "Supprimer");
                foreach (string scenario in scenarioselection.getScenarios())
                    {
                    FMTCache.remove(primary,scenario);
                    }
                }
            Refresh();
            //updateAllformula();
            }
        public static void Refresh()
        {
            if (_ribbonUi != null) { _ribbonUi.Invalidate(); }
            //Microsoft.Office.Interop.Excel.Application excelApp = ExcelDna.Integration.ExcelDnaUtil.Application as Microsoft.Office.Interop.Excel.Application;
            //excelApp.CalculateFull();
        }
        public void updateAll(IRibbonControl control1)
            {
            Microsoft.Office.Interop.Excel.Application excelApp = ExcelDna.Integration.ExcelDnaUtil.Application as Microsoft.Office.Interop.Excel.Application;
            excelApp.CalculateFull();
            }

        public void updateActive(IRibbonControl control1)
        {
            Microsoft.Office.Interop.Excel.Application excelApp = ExcelDna.Integration.ExcelDnaUtil.Application as Microsoft.Office.Interop.Excel.Application;
            Microsoft.Office.Interop.Excel.Worksheet sheet = excelApp.ActiveSheet;
            sheet.UsedRange.Dirty();
            sheet.Calculate();
        }
        public void toGithub(IRibbonControl control1)
        {
            System.Diagnostics.Process.Start("https://github.com/Bureau-du-Forestier-en-chef/FMT");
        }

        public void toWebDoc(IRibbonControl control1)
        {
            System.Diagnostics.Process.Start("https://bureau-du-forestier-en-chef.github.io/FMTdocs/fr/");
        }

        public void getFonctions(IRibbonControl control1)
        {
            FunctionsDropDown dropdown = new FunctionsDropDown();
        }

        public string CombogetModels(IRibbonControl control,int index)
        {
            if (!FMTCache.empty())
            {
                int id = 0;
                foreach (string primary in (List<string>)FMTCache.callFMTMethod("getprimaries", null))
                {
                    foreach (string scenario in (List<string>)FMTCache.callFMTMethod("getscenarios", new object[] { primary }))
                    {
                        if (index == id)
                        {
                            return primary + " " + scenario;
                        }
                        ++id;
                    }
                }
            }
            return "";
        }
        public void SaveModelChoice(IRibbonControl control, string selectedId, int selectedIndex)
        {
            if (!FMTCache.empty())
            {
                int id = 0;
                foreach (string primary in (List<string>)FMTCache.callFMTMethod("getprimaries", null))
                {
                    foreach (string scenario in (List<string>)FMTCache.callFMTMethod("getscenarios", new object[] { primary }))
                    {
                        if (selectedIndex == id)
                        {
                            FMT.appendToFormulaValue(primary+";"+scenario);
                            return;
                        }
                        ++id;
                    }
                }
            }
        }

        public int CombogetCount(IRibbonControl control)
        {
            return FMTCache.size();
        }

        public Bitmap GetImage(IRibbonControl control)
        {
            switch (control.Id)
            {
                case "AjouterAvecSolution": return new Bitmap(FMTExcel.Properties.Resources._2714_color);
                case "AjouterSansSolution": return new Bitmap(FMTExcel.Properties.Resources.check);
                case "Retirer": return new Bitmap(FMTExcel.Properties.Resources._274C_color);
                case "TMJ": return new Bitmap(FMTExcel.Properties.Resources._1F6BD_color);
                case "AMJ": return new Bitmap(FMTExcel.Properties.Resources._1FAA0_color);
                case "github": return new Bitmap(FMTExcel.Properties.Resources.E045_color);
                case "docFMT": return new Bitmap(FMTExcel.Properties.Resources._1F4D6_color);
                case "docF": return new Bitmap(FMTExcel.Properties.Resources._2753_color);
                default: return null;
            }
        }

        public override void OnBeginShutdown(ref Array custom)
            {
            if (!FMTCache.empty())
                {
                FMTCache.clearTempFiles();
                }
            }
    }
}
