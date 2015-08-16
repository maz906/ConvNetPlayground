using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace ReferenceImageSelector
{
    public static class Extensions
    {
        public static ImageSource Next(this IEnumerator<ImageSource> image)
        {
            ImageSource source = image.Current;
            try
            {
                image.MoveNext();
            }
            catch { return null; }
            return source;
        }
    }
}
