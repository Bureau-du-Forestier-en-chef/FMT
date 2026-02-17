using System;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;

namespace FMTXLL
{
    public class ErrorWindow : System.Windows.Forms.Form
    {
        public ErrorWindow(string message, string windowname)
        {
            FormBorderStyle = System.Windows.Forms.FormBorderStyle.Sizable;
            IntPtr Hicon = FMTExcel.Properties.Resources.LogoFMT.GetHicon();
            Icon newIcon = Icon.FromHandle(Hicon);
            Icon = newIcon;
            Text = windowname;
            System.Windows.Forms.ListBox lbox = new System.Windows.Forms.ListBox();
            foreach (string value in message.Split('\n'))
            {
                lbox.Items.Add(value);
            }
            MinimumSize = new System.Drawing.Size(windowname.Count(),
                                            lbox.Items.Count);
            lbox.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            AutoSize = false;
            lbox.Dock = DockStyle.Fill;
            lbox.IntegralHeight = false; 
            lbox.ScrollAlwaysVisible = true;
            lbox.HorizontalScrollbar = true;
            if (lbox.Items.Count > 0)
                {
                lbox.TopIndex = lbox.Items.Count - 1;
                lbox.SelectedIndex = lbox.Items.Count - 1;
            }
            Controls.Add(lbox);
        }

    }
}
