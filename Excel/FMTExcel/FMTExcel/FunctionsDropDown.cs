using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.Drawing;
using ExcelDna.Integration;

namespace FMTXLL
{
    public class FunctionsDropDown
    {
      private System.Windows.Forms.Form form;
      private Dictionary<string, ExcelFunctionAttribute> attributes;
      private Dictionary<string, List<ExcelDna.Integration.ExcelArgumentAttribute>> arguments;
        private void setFunctionsAttributes()
        {
            attributes = new Dictionary<string, ExcelFunctionAttribute>();
            arguments = new Dictionary<string, List<ExcelDna.Integration.ExcelArgumentAttribute>>();
            FMT myObject = new FMT();
            System.Reflection.MemberInfo[] myMemberInfo;
            Type myType = myObject.GetType();
            myMemberInfo = myType.GetMembers();
            foreach (System.Reflection.MethodInfo mInfo in myType.GetMethods())
            {
                if (!mInfo.Name.Contains("Auto") && !mInfo.Name.Contains("ToString")
                    && !mInfo.Name.Contains(".") && !mInfo.Name.Contains("Get") && !mInfo.Name.Contains("Set") && !mInfo.Name.Contains("Equals"))
                {
                    arguments[mInfo.Name] = new List<ExcelDna.Integration.ExcelArgumentAttribute>();
                    foreach (System.Reflection.ParameterInfo par in mInfo.GetParameters())
                    {
                        foreach (Attribute attr in Attribute.GetCustomAttributes(par))
                        {
                            arguments[mInfo.Name].Add((ExcelDna.Integration.ExcelArgumentAttribute)attr);
                        }
                            
                    }
                        
                    // Iterate through all the Attributes for each method.
                    foreach (Attribute attr in Attribute.GetCustomAttributes(mInfo))
                    {
                        attributes[mInfo.Name] = (ExcelFunctionAttribute)attr;
                       // System.Windows.Forms.MessageBox.Show(excelatt.Name+" "+ excelatt.Category+" "+ excelatt.Description);
                    }
                }

            }
        }
        private void MouseLeave(object sender, System.EventArgs e)
        {
            if (Cursor.Position.X < form.Location.X
                || Cursor.Position.Y < form.Location.Y
                || Cursor.Position.X > form.Location.X + form.Width
                || Cursor.Position.Y > form.Location.Y + form.Height)
            {
                form.Close();
            }
           
        }
        private void ClickClose(object sender, System.EventArgs e)
        {
                form.Close();
        }
        private void selection(object sender, EventArgs e)
        {
            ComboBox senderComboBox = (ComboBox)sender;
            // Change the length of the text box depending on what the user has 
            // selected and committed using the SelectionLength property.
            if (senderComboBox.SelectedIndex >= 0)
            {
                string name = senderComboBox.Items[senderComboBox.SelectedIndex].ToString();
                string category = attributes[name].Category;
                string description = attributes[name].Description;
                string returnelement = attributes[name].HelpTopic;
                string argumentstr = "";
                int id = 1;
                foreach (ExcelDna.Integration.ExcelArgumentAttribute arg in arguments[name])
                {
                    argumentstr += ("(" + id + ") " + arg.Name + ": " + System.Environment.NewLine + arg.Description + System.Environment.NewLine + System.Environment.NewLine);
                    ++id;
                }

                System.Windows.Forms.Label catlabel = form.Controls.Find("cat", true).FirstOrDefault() as System.Windows.Forms.Label;
                if (catlabel == null)
                {
                    catlabel = new Label();
                }
                catlabel.Location = new Point(senderComboBox.Location.X + 150, senderComboBox.Location.Y);
                catlabel.Text = "Catégorie: " + category;
                catlabel.Name = "cat";
                catlabel.AutoSize = true;
                catlabel.BackColor = Color.LightGray;

                System.Windows.Forms.Label deslabel = form.Controls.Find("deslabel", true).FirstOrDefault() as System.Windows.Forms.Label;
                if (deslabel == null)
                {
                    deslabel = new Label();
                }
                deslabel.Location = new Point(senderComboBox.Location.X + 150, senderComboBox.Location.Y + 20);
                deslabel.Text = "Description:";
                deslabel.Name = "deslabel";
                deslabel.AutoSize = true;
                deslabel.BackColor = Color.LightGray;

                System.Windows.Forms.Label parlabel = form.Controls.Find("parlabel", true).FirstOrDefault() as System.Windows.Forms.Label;
                if (parlabel == null)
                {
                    parlabel = new Label();
                }
                parlabel.Location = new Point(senderComboBox.Location.X + 150, senderComboBox.Location.Y + 90);
                parlabel.Text = arguments[name].Count() + " Paramètres:";
                parlabel.Name = "parlabel";
                parlabel.AutoSize = true;
                parlabel.BackColor = Color.LightGray;

                System.Windows.Forms.TextBox textBox1 = form.Controls.Find("description", true).FirstOrDefault() as System.Windows.Forms.TextBox;

                bool needtoadd = false;
                if (textBox1 == null)
                {
                    needtoadd = true;
                    textBox1 = new System.Windows.Forms.TextBox();
                }
                //form.SuspendLayout();
                textBox1.Name = "description";
                textBox1.AcceptsReturn = true;
                textBox1.AcceptsTab = true;
                //textBox1.Dock = System.Windows.Forms.DockStyle.Fill;
                textBox1.Multiline = true;
                textBox1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
                textBox1.Text = description;
                textBox1.Visible = true;
                textBox1.ReadOnly = true;
                textBox1.BackColor = System.Drawing.SystemColors.Window;
                textBox1.Size = new Size(200, 50);
                textBox1.Location = new System.Drawing.Point(senderComboBox.Location.X + 150, senderComboBox.Location.Y + 40);


                System.Windows.Forms.TextBox textBox2 = form.Controls.Find("parametres2", true).FirstOrDefault() as System.Windows.Forms.TextBox;
                if (textBox2 == null)
                {
                    textBox2 = new System.Windows.Forms.TextBox();
                }
                //form.SuspendLayout();
                textBox2.Name = "parametres2";
                textBox2.AcceptsReturn = true;
                textBox2.AcceptsTab = true;
                //textBox1.Dock = System.Windows.Forms.DockStyle.Fill;
                textBox2.Multiline = true;
                textBox2.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
                textBox2.Text = argumentstr;
                textBox2.Visible = true;
                textBox2.ReadOnly = true;
                textBox2.BackColor = System.Drawing.SystemColors.Window;
                textBox2.Size = new Size(200, 50);
                textBox2.Location = new System.Drawing.Point(senderComboBox.Location.X + 150, senderComboBox.Location.Y + 110);

                System.Windows.Forms.Label returnlabel = form.Controls.Find("returnlabel", true).FirstOrDefault() as System.Windows.Forms.Label;
                if (returnlabel == null)
                {
                    returnlabel = new Label();
                }
                returnlabel.Location = new Point(senderComboBox.Location.X + 150, senderComboBox.Location.Y + 160);
                returnlabel.Text = "Retour:";
                returnlabel.Name = "returnlabel";
                returnlabel.AutoSize = true;
                returnlabel.BackColor = Color.LightGray;

                System.Windows.Forms.TextBox textBox3 = form.Controls.Find("retourdes", true).FirstOrDefault() as System.Windows.Forms.TextBox;
                if (textBox3 == null)
                {
                    textBox3 = new System.Windows.Forms.TextBox();
                }
                //form.SuspendLayout();
                textBox3.Name = "retourdes";
                textBox3.AcceptsReturn = true;
                textBox3.AcceptsTab = true;
                //textBox1.Dock = System.Windows.Forms.DockStyle.Fill;
                textBox3.Multiline = true;
                textBox3.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
                textBox3.Text = returnelement;
                textBox3.Visible = true;
                textBox3.ReadOnly = true;
                textBox3.BackColor = System.Drawing.SystemColors.Window;
                textBox3.Size = new Size(200, 50);
                textBox3.Location = new System.Drawing.Point(senderComboBox.Location.X + 150, senderComboBox.Location.Y + 180);
                if (needtoadd)
                {
                    form.Controls.Add(catlabel);
                    form.Controls.Add(deslabel);
                    form.Controls.Add(textBox1);
                    form.Controls.Add(parlabel);
                    form.Controls.Add(textBox2);
                    form.Controls.Add(returnlabel);
                    form.Controls.Add(textBox3);
                }

                //form.ResumeLayout(false);
                //form.PerformLayout();
                // System.Windows.Forms.MessageBox.Show(name + " " + category + " " + description);
                //FMT.Appendtoformulavalue("=" + name + "(");

            }

        }

        public FunctionsDropDown()
        {
            
            
            form = new System.Windows.Forms.Form();
            form.FormBorderStyle = FormBorderStyle.FixedSingle;
            form.MaximizeBox = false;
            form.MinimizeBox = false;
            //form.AutoSize = true;
            form.Size = new Size(350,250);
            form.ControlBox = false;
            form.WindowState = FormWindowState.Normal;
            form.StartPosition = FormStartPosition.Manual;
            form.Location = new System.Drawing.Point(System.Windows.Forms.Cursor.Position.X-2, System.Windows.Forms.Cursor.Position.Y-2);
            ToolTip toolTip1 = new ToolTip();
            toolTip1.AutoPopDelay = 0;
            toolTip1.InitialDelay = 0;
            toolTip1.ReshowDelay = 0;
            toolTip1.ShowAlways = true;
            
            System.Windows.Forms.ComboBox ComboBox1 = new ComboBox();
            toolTip1.SetToolTip(ComboBox1, "Information sur les fonctions");
            /* ComboBox1.Name = "ComboBox1";
             ComboBox1.DroppedDown = true;*/
            setFunctionsAttributes();
             foreach (KeyValuePair<string, ExcelFunctionAttribute> entry in attributes)
                 {
                 ComboBox1.Items.Add(entry.Key);
                 }
             ComboBox1.SelectedIndex = 1;
             ComboBox1.TabIndex = 0;
            //ComboBox1.Text = "Typical";
            form.Controls.Add(ComboBox1);
            ComboBox1.SelectionChangeCommitted += new System.EventHandler(this.selection);
            //form.MouseLeave += new System.EventHandler(this.MouseLeave);
            //ComboBox1.MouseLeave += new System.EventHandler(this.MouseLeave);
            Button closeButton = new Button();
            closeButton.Text = "Fermer";
            closeButton.Click += new System.EventHandler(this.ClickClose);
            closeButton.Location = new Point(0, 220);
            form.Controls.Add(closeButton);
            
            form.Show();
        }

    }
}
