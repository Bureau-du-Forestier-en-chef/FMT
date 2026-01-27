using ExcelDna.Integration;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FMTXLL
{
    internal static class Optional
    {
        internal static bool Check(object arg, bool defaultValue)
        {
            if (arg is bool)
                return (bool)arg;
            else if (arg is ExcelMissing)
                return defaultValue;
            else
                return defaultValue;  // Or whatever you want to do here....

        }
        internal static String Check(object arg, String defaultValue)
        {
            if (arg is String)
                return (String)arg;
            else if (arg is ExcelMissing)
                return defaultValue;
            else
                return defaultValue;  // Or whatever you want to do here....

        }
    }
}
