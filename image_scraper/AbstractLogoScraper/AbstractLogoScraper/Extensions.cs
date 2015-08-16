using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AbstractScraper;
using System.IO;
using System.Collections.ObjectModel;

namespace ExtensionMethods
{
    /// <summary>
    /// These extension methods act like instance methods.
    /// </summary>
    public static class Extensions
    {
        /// <summary>
        /// A set of words to ignore when looking for keywords associated with a brand,
        /// i.e. words which have unusually high frequency in the titles of the results
        /// returned by a search engine.
        /// </summary>
        static readonly HashSet<string> ignore = new HashSet<string> { "i", "you", "he", "she", "it", "we", "they", 
                                                                            "him", "her", "it's", "us", "them", "theirs",
                                                                            "is", "can", "will", "would", "should", "could",
                                                                            "be", "do", "say", "get", "make", "go", "know", "take",
                                                                            "the", "a", "an", "to", "from", "with", "about",
                                                                            "after", "against", "at", "through", "throughout", 
                                                                            "up", "upon", "over", "since", "for", "except",
                                                                            "like", "near", "of", "off", "on", "out",
                                                                            "and", "de", "logo", "see", "by", "in", "around",
                                                                            "about", "because" };
        /// <summary>
        /// Split the title of a query by these delimiters. Should be using regexes...
        /// </summary>
        static readonly string[] split = { ",", " ", ".", "-", "..", "...", "....", "--", "  ", "   ", "    " };

        /// <summary>
        /// Given a set of IQueries, this returns the set of keywords that occurred
        /// in the titles of those queries.
        /// </summary>
        /// <param name="list"></param>
        /// <returns></returns>
        public static HashSet<string> keywords(this HashSet<IQuery> list)
        {
            HashSet<string> keywords = new HashSet<string>();
            foreach (IQuery q in list)
                foreach (string word in q.title.ToLower().Trim().Split(split, StringSplitOptions.RemoveEmptyEntries))
                    if (!ignore.Contains(word))
                        keywords.Add(word);
            return keywords;
        }

        /// <summary>
        /// Given a set of IQueries, this returns the set of urls that were
        /// returned for those IQueries.
        /// </summary>
        /// <param name="list"></param>
        /// <returns></returns>
        public static HashSet<string> urls(this HashSet<IQuery> list)
        {
            HashSet<string> urls = new HashSet<string>();
            foreach (IQuery q in list)
                urls.Add(q.url);
            return urls;
        }

        /// <summary>
        /// The assumption is that file represents a string to a file in a 
        /// brand folder in the images directory. This function obtains the number
        /// on the end of the filename.
        /// </summary>
        /// <param name="file"></param>
        /// <returns></returns>
        public static int file_num(this string file)
        {
            try
            {
                return Int32.Parse(Path.GetFileNameWithoutExtension(file).Split('_').Last());
            }
            catch
            {
                //The only way an error is thrown should be when file's filename doesn't end in
                //a number. In that case, return something which won't affect the rankings
                //needed in largest_img (method in AbstractLogoScraper).
                return 0;
            }
        }

        /// <summary>
        /// Used when creating the brand directories in the image directory. All
        /// illegal characters are removed and replaced with the empty string.
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        public static string sanitizePath(this string path)
        {
            string invalid = new string(Path.GetInvalidFileNameChars()) + new string(Path.GetInvalidPathChars());

            foreach (char c in invalid)
            {
                path = path.Replace(c.ToString(), "");
            }
            return path;
        }

        public static string toStr(this Collection<string> collec)
        {
            StringBuilder sb = new StringBuilder();
            foreach (string tag in collec)
            {
                sb.Append(tag + ",");
            }
            return sb.ToString();
        }
    }
}
