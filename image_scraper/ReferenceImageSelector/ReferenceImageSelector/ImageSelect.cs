using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Windows.Media;
using System.Windows.Media.Imaging;



namespace ReferenceImageSelector
{
    public class ImageSelect
    {
        const string brandPath = "../../../../../images/train/";
        const string pathToStore = "../../../../../referenceImages/";

        public const int referenceNumber = 1;
        public int currentCount { get; set; }

        public ImageSelect()
        {
            currentCount = 0;
        }
        /// <summary>
        /// Returns an IEnumerable of all the images in brandPath. Image names are of
        /// the form brandPath/brand/image_#.format
        /// </summary>
        /// <returns></returns>
        public IEnumerable<IEnumerable<string>> GetImages()
        {
            List<IEnumerable<string>> images = new List<IEnumerable<string>>();
            foreach (string brand in Directory.EnumerateDirectories(brandPath))
            {
                string brandRoot = string.Format("{0}/", brand);
                IEnumerable<string> temp = Directory.GetFiles(brandRoot);
                images.Add(temp);
                brandRoot = brandRoot.Split(new string[] { "/" }, StringSplitOptions.RemoveEmptyEntries).Last();
                if (!Directory.Exists(pathToStore + brandRoot))
                    Directory.CreateDirectory(pathToStore + brandRoot);
            }
            return images;
        }

        /// <summary>
        /// Takes a list of reference logo images and stores them in pathToStore/brand. The
        /// image names are assumed to be of the form brandPath/brand/image_#.format
        /// </summary>
        /// <param name="images"></param>
        public void StoreImages(IEnumerable<string> images)
        {
            string brand = this.GetBrand(images.First());
            images.Select(s => { 
                                    Directory.Move(s, string.Format("{0}{1}/{2}", pathToStore, brand, s)); 
                                    return s; 
                                });
        }

        /// <summary>
        /// Given a path of the form /../../brand/image_1.jpg, returns brand.
        /// </summary>
        /// <param name="image"></param>
        /// <returns></returns>
        public string GetBrand(string image)
        {
            return Path.GetDirectoryName(image).Split(new string[] {"/", "//", "\\", "\\\\"}, StringSplitOptions.RemoveEmptyEntries).Last();
        }

        public int GetLargest(string path)
        {
            string largestFile;
            try
            {
                largestFile = Directory.GetFiles(path).OrderBy(s => -1 * Int32.Parse(Path.GetFileNameWithoutExtension(s).Split('_').Last())).First();
            }
            catch { return 0; }
            return int.Parse(Path.GetFileNameWithoutExtension(largestFile).Split('_').Last()); //this shouldn't throw an exception...
        }

        public void MoveImage(ImageSource image)
        {
            string source = ((BitmapImage)image).UriSource.LocalPath;
            string brand = GetBrand(source);
            int currentCount = GetLargest(pathToStore + brand) + 1;
            this.currentCount = currentCount;
            if (currentCount > referenceNumber)
                return;
            File.Copy(source, string.Format("{0}{1}/ref_{2}{3}", pathToStore, GetBrand(source), currentCount, Path.GetExtension(source))); 
        }

        public IEnumerator<ImageSource> DisplayImages()
        {
            foreach (IEnumerable<string> brandFiles in GetImages())
            {
                foreach (string image in brandFiles) 
                {
                    if (currentCount >= referenceNumber)
                        yield break;

                    //display the image and record the uri
                    yield return new BitmapImage(new Uri(System.IO.Path.GetFullPath(image))); //replace with LogoImage 
                    
                    
                }

            }
        }
    }
}
