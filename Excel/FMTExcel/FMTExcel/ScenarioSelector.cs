using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Net.Mime.MediaTypeNames;

namespace FMTXLL
{
    public class ScenarioSelector
    {
        private System.Windows.Forms.Form form;
        private CheckedListBox checkbox;
        private System.Windows.Forms.Button select;
        List<System.Windows.Forms.TextBox> LengthBox;
        List<System.Windows.Forms.Label> LabelsBox;
        List<string> selected;
        List<int> Length;
        private void scenario_click(object sender, System.EventArgs e)
        {
            selected = new List<string>();
            foreach (var item in checkbox.CheckedItems)
            {
                selected.Add(item.ToString());
            }
            form.Close();
        }

        private void lengthSelected(object sender, System.EventArgs e)
        {
            Length = new List<int>();
           
            foreach (System.Windows.Forms.TextBox TEXT in LengthBox)
            {
                int Value = 0;
                int.TryParse(TEXT.Text, out Value);
                Length.Add(Value);
            }
            form.Close();
        }
        private void Form1_Load(object sender, EventArgs e)
        {
            // no smaller than design time size
            form.MinimumSize = new System.Drawing.Size(form.Width, form.Height);

            // no larger than screen size
            form.MaximumSize = new System.Drawing.Size(Screen.PrimaryScreen.Bounds.Width, 
                                Screen.PrimaryScreen.Bounds.Height);

            form.AutoSize = true;
            form.AutoSizeMode = AutoSizeMode.GrowAndShrink;

            // rest of your code here...
        }

        private void FillInLength()
        {
            form = new System.Windows.Forms.Form();
            IntPtr Hicon = FMTExcel.Properties.Resources.LogoFMT.GetHicon();
            Icon newIcon = Icon.FromHandle(Hicon);
            form.Icon = newIcon;
            LengthBox = new List<System.Windows.Forms.TextBox>();
            LabelsBox = new  List<System.Windows.Forms.Label>();
            form.Text = "Horizon de planification";
            var panel = new TableLayoutPanel();
           
            //panel.ColumnCount = 2;
           // panel.RowCount = getScenarios().Count;
            panel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 10F));
            panel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 10F));
            int rowNumber = 0;
            foreach (string scenario in getScenarios())
            {
                panel.RowStyles.Add(new RowStyle(SizeType.Percent, 10F));
                System.Windows.Forms.Label TheLabel = new System.Windows.Forms.Label();
                TheLabel.Text = scenario;
                TheLabel.AutoSize = true;
                LabelsBox.Add(TheLabel);
                panel.Controls.Add( TheLabel,0, rowNumber);
                System.Windows.Forms.TextBox Thebox = new System.Windows.Forms.TextBox();
                Thebox.Text = "0";
                LengthBox.Add(Thebox);
                panel.Controls.Add(Thebox, 1, rowNumber);
                ++rowNumber;
            }
            select = new System.Windows.Forms.Button();
            select.Enabled = true;
            select.Text = "Enregistrer";
            select.Visible = true;
            select.AutoSize = true;
            select.Width = 300;
            /*select.Height = 10;*/
            select.Anchor = System.Windows.Forms.AnchorStyles.Left;
            panel.Controls.Add(select);
            form.Controls.Add(panel);
            // select.Location = new System.Drawing.Point(100, 200);
            form.ClientSize = new System.Drawing.Size(250,100);
            select.Click += new System.EventHandler(this.lengthSelected);
            //form.Load += new System.EventHandler(this.Form1_Load);
            form.ShowDialog();
        }
        public ScenarioSelector(List<string> basescenario, string nomdelaselection, string nomdubouton,bool getLength=false)
        {
            form = new System.Windows.Forms.Form();
            IntPtr Hicon = FMTExcel.Properties.Resources.LogoFMT.GetHicon();
            Icon newIcon = Icon.FromHandle(Hicon);
            form.Icon = newIcon;
            checkbox = new CheckedListBox();
            form.Text = nomdelaselection;
            foreach (string scenario in basescenario)
            {
                checkbox.Items.Add(scenario);
            }
            checkbox.IntegralHeight = true;
            checkbox.Enabled = true;
            checkbox.CheckOnClick = true;
            checkbox.Width = 500;
            checkbox.Height = 200;
            checkbox.Location = new System.Drawing.Point(0, 0);
            //form.Controls.Add(checkbox);
            select = new System.Windows.Forms.Button();
            select.Enabled = true;
            select.Text = nomdubouton;
            select.Visible = true;
            select.AutoSize = true;
            select.Width = 300;
            select.Height = 10;
            select.Location = new System.Drawing.Point(100, 200);
            //form.Controls.Add(selectionner);
            form.ClientSize = new System.Drawing.Size(500, 240);
            form.Controls.AddRange(new System.Windows.Forms.Control[] { checkbox, select });
            select.Click += new System.EventHandler(this.scenario_click);
            form.ShowDialog();
            if (getLength)
                {
                FillInLength();
                }
        }

        public List<string> getScenarios()
        {
            if (selected == null)
            {
                return new List<string>();
            }
            return selected;
        }

        public List<int> getScenariosLength()
        {
            if (selected == null)
            {
                return new List<int>();
            }
            return Length;
        }
    }
}

