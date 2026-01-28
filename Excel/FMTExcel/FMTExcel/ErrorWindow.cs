using System;
using System.Drawing;

namespace FMTXLL
{
    public class ErrorWindow : System.Windows.Forms.Form
    {
        public ErrorWindow(string message, string windowname)
        {
            FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            IntPtr Hicon = FMTExcel.Properties.Resources.LogoFMT.GetHicon();
            Icon newIcon = Icon.FromHandle(Hicon);
            Icon = newIcon;
            Text = windowname;
            System.Windows.Forms.ListBox lbox = new System.Windows.Forms.ListBox();
            int width = 0;
            foreach (string value in message.Split('\n'))
            {
                width = Math.Max(width, value.Length * 10);
                lbox.Items.Add(value);
            }
            width = Math.Max(500, width + 10);
            lbox.Size = new System.Drawing.Size(width, 700);
            ClientSize = new System.Drawing.Size(width, 700);
            Controls.Add(lbox);
        }

    }
}
