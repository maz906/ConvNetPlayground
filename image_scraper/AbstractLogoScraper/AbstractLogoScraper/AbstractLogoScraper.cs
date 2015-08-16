using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.IO;
using System.Net;
using ExtensionMethods;
using AbstractScraper;

namespace AbstractScraper
{
    /// <summary>
    /// Any class which implements AbstractLogoScraper is meant to gather Queries
    /// which contain a url and title.
    /// 
    /// lolololololol this is actually quite concrete
    /// </summary>
    public abstract class AbstractLogoScraper : ILogoScraper
    {
        public string api_key { get; set; }
        public string root_url { get; set; }
        public int time_between_calls { get; set; }
        public int max_refinement { get; set; }
        /// <summary>
        /// This is meant to limit the number of worker threads, which will thusly
        /// limit the number of downloads occurring at once.
        /// </summary>
        public int max_workerThreads { get; set; }
        /// <summary>
        /// Each API seems to have a limit on the maximum number of results returned
        /// per query. Set this to that.
        /// </summary>
        public int maxResultsPerQuery { get; set; }

        /// <summary>
        /// All the logo scrapers will have the same path and access to a brand_name.txt
        /// file which has a list of brand names.
        /// </summary>
        public const string path = "../../../../images";
        public string brand_file { get; set; }

        /// <summary>
        /// Loads brands from brand_file into a list of strings.
        /// </summary>
        /// <param name="brandFile"></param>
        /// <returns></returns>
        public HashSet<string> brand_list() 
        {
            HashSet<string> brands = new HashSet<string>();
            string line;
            using (StreamReader read = new StreamReader(brand_file))
            {
                while ((line = read.ReadLine()) != null)
                    brands.Add(line.ToLower().Trim());
            }
            return brands;
        }


        public void download_images(string brand, HashSet<string> urls)
        {
            //don't start naming files at image_count = 0 but at the most
            //recent file.
            int image_count = largest_img(brand_path(brand)) + 1;

            foreach (string url in urls)
            {
                //remove anything after the first ? in the url, and find the extension.
                //Store the truncated_url (actually Uri) for later use
                string ext;
                Uri truncated_url = truncate_url(url, out ext);

                Console.WriteLine("Downloading from: " + truncated_url);
                //Create a new WebClient to deal with a single download.
                using (WebClient local = new WebClient())
                {
                    //copy the current value of image_count so we can use it to name the image
                    //increment image_count. We need a lock due to the asynchronous method.
                    int count;
                    lock (local)
                    {
                        count = image_count;
                        image_count++;
                    }
                    //save, for example, an image to "../../../../images/brand/image_30.png
                    try
                    {
                        //We don't want to download too many files at once (maybe ten at a time, at most).
                        //This is controlled by setting the maximum number of threads which can call this method.
                        local.DownloadFile(truncated_url, string.Format("{0}/image_{1}{2}", brand_path(brand), count, ext));
                    }
                    catch (Exception e)
                    {
                        //want to know that the download failed and why, but continue
                        Console.WriteLine("Download failed: " + e.Message);
                        continue;
                    }
                }
            }
        }

        /// <summary>
        /// Grabs the most recent image in the path.
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        private int largest_img(string path)
        {
            //Rank the files from largest to smallest according to their number, e.g. image_39.png
            IOrderedEnumerable<string> files = Directory.GetFiles(path).OrderBy(s => -1*s.file_num());
            if (files.Count() == 0)
                return -1;

            //Take out the first file. Any files without image numbers will have 
            //been assigned a 0, so they will have ended up in the middle of the
            //rankings or last.
            return files.First().file_num();
        }

        /// <summary>
        /// Only takes everything before the first '?' in a URL.
        /// 
        /// INVARIANT: the url is assumed to have a file extension. The extension
        /// is passed to an out.
        /// </summary>
        /// <param name="url"></param>
        /// <param name="ext"></param>
        /// <returns></returns>
        private Uri truncate_url(string url, out string ext)
        {
            url = url.Split('?')[0];
            ext = Path.GetExtension(url.sanitizePath());
            return new Uri(url);
        }

        public HashSet<string> refined_searchTerms(string brand, HashSet<IQuery> urls)
        {   
            //Create a histogram of frequency of each word.         
            Dictionary<string, Int32> searchData = new Dictionary<string, Int32>();
            foreach (var group in urls.keywords().GroupBy(s => s))
                searchData.Add(group.Key, group.Count());

            //Sort searchData by largest value (notice the reverse direction of
            //the sort function).
            List<KeyValuePair<string, int>> sorted = searchData.ToList();
            sorted.Sort((p1, p2) => p2.Value.CompareTo(p1.Value));
            //Find the top this.max_refinement keywords.
            HashSet<string> top_keywords = new HashSet<string>(); 
            int idx = 0;
            double tot = total(sorted);
            foreach (KeyValuePair<string, int> pair in sorted)
            {
                //If the candidate significant word is the same as brand, discard it.
                if (brand == pair.Key)
                    continue;

                //Otherwise, if the word occurs more frequently than 1/this.max_refinement,
                //and it's not already in the hashset, add it.
                if (pair.Value / tot > 0.1 && !top_keywords.Contains(pair.Key))
                {
                    top_keywords.Add(pair.Key);
                    idx++;
                }   

                //If we've added max_refinement keywords, break from the loop.
                if (idx >= max_refinement)
                    break;
            }

            return top_keywords;
        }

        /// <summary>
        /// Calculate the total number of items in the histogram.
        /// </summary>
        /// <param name="histogram"></param>
        /// <returns></returns>
        private static int total(List<KeyValuePair<string, int>> histogram)
        {
            int total = 0;
            foreach (KeyValuePair<string, int> pair in histogram)
            {
                total += pair.Value;
            }
            return total;
        }

        /// <summary>
        /// Accesses the URLs associated with a brand. The exact implementation will
        /// vary from API to API.
        /// </summary>
        /// <param name="brand"></param>
        /// <returns></returns>
        public abstract HashSet<IQuery> get_urls(string brand, params object[] args);

        public void run()
        {
            this.save_images(this.brand_list());
        }


        public void save_images(HashSet<string> brands)
        {
            //get the set of IQueries from the search engine API. This will act as 
            //a list of URLs to download.
            Dictionary<string, HashSet<IQuery>> brandToQuery = download_list(brands, true);

            //multithread based on the brands. No asynchronous downloading. Set the max threads in
            //the pool so that we have at most max_workerThreads downloads at any instant.
            ThreadPool.SetMaxThreads(max_workerThreads, 5);
            foreach (string brand in brands)
                ThreadPool.QueueUserWorkItem(new WaitCallback((o) => download_images(brand,
                    brandToQuery[brand].urls())));
        }

        public Dictionary<string, HashSet<IQuery>> download_list(HashSet<string> brands, params object[] param)
        {
            Dictionary<string, HashSet<IQuery>> brandToQuery = new Dictionary<string, HashSet<IQuery>>();
            foreach (string brand in brands)
            {
                //Create a directory for the images
                string brand_path = this.brand_path(brand);
                if (!Directory.Exists(brand_path))
                    Directory.CreateDirectory(brand_path);

                //create an entry from brand to list of URLs of that brand's logo
                if (!brandToQuery.Keys.Contains(brand))
                    brandToQuery.Add(brand, new HashSet<IQuery>());

                //Gather all the URLs.
                HashSet<IQuery> brand_urls = get_urls(brand, param); //skip none at first
                brandToQuery[brand].UnionWith(brand_urls);
                //HashSet<string> keywords = this.refined_searchTerms(brand, brandToQuery[brand]);

                ////if we found no keywords, search the next brand name a few more times,
                ////skipping the first results. skip is the number of multiples of maxResultsPerQuery
                ////are leftover that we need to fill up.
                //int skip = max_refinement - keywords.Count;
                ////If skip == 0, i.e. we've met the quota, we shouldn't be skipping any. If we
                ////haven't met the quota, we should be skipping the first skip*maxResultsPerQuery results.
                //int mults = skip == 0 ? 1 : skip; //how many 

                ////Add an element so that even if keywords is empty, it will go through the
                ////loop to meet the quota.
                //if (keywords.Count == 0)
                //    keywords.Add("");

                //foreach (string keyword in keywords)
                //{
                //    brandToQuery[brand].UnionWith(get_urls(string.Format("{0} {1} logo", brand, keyword), mults, skip));
                //}

                //Save the URLs to a file.
                this.SaveURLAsync(brand, brandToQuery[brand]);

            }

            return brandToQuery;
        }

        /// <summary>
        /// Return the string representing the path to the brand.
        /// </summary>
        /// <param name="brand"></param>
        /// <returns></returns>
        private string brand_path(string brand)
        {
            return string.Format("{0}/{1}", path, brand.sanitizePath());
        }

        /// <summary>
        /// Saves the queries to a urls.txt file in the brand folder.
        /// </summary>
        /// <param name="brand"></param>
        /// <param name="urls"></param>
        public async void SaveURLAsync(string brand, HashSet<IQuery> urls) 
        {
            Console.WriteLine("Saving URLs to: {0}", brand_path(brand) + "/urls.txt");
            WriteToFile:
                StreamWriter writer;
                try
                {
                    writer = new StreamWriter(brand_path(brand) + "/urls.txt", true);
                } catch
                {
                    //If there is an error, it's probably since the file is already
                    //being written to by something else. Sleep, then check again.
                    Thread.Sleep(1000);
                    goto WriteToFile;
                }

            //Write just the URL
            foreach (IQuery query in urls)
            {
                await writer.WriteLineAsync(query.title + "::::" + query.url);
            }

            //Close the writer!
            writer.Close();
        }
    }

    public abstract class AbstractQuery : IQuery
    {
        public string title { get; set; }
        public string url { get; set; }

        public override bool Equals(object obj)
        {
            if (!(obj is AbstractQuery))
                return false;
            AbstractQuery temp = (AbstractQuery)obj;
            return temp.url == this.url;
        }

        public override int GetHashCode()
        {
            return this.url.GetHashCode();
        }

    }
   
}
