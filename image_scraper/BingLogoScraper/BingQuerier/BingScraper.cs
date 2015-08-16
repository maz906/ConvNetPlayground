using System;
using System.Collections.Generic;
using System.Collections.Concurrent;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.IO;
using Bing;
using AbstractScraper;
using ExtensionMethods;

/// <summary>
/// Scrape logos. For each brand, there are at most multiplesOfFifty*options.length number of
/// queries. Default values currently are multiplesOfFifty = 20 and options.length = 1, so 20 
/// queries per brand. 
/// <author></author>
/// </summary>
namespace BingLogoScraper
{
    public class BingScraper : AbstractLogoScraper
    {

        //We can get at most 50 results per search, and to get more than 50 we'll want to
        //skip 50 images at a time.
        public int maxImagesPerSkip { get; set; }
        //Pick how many multiples of fifty to search for each image (up to 20, or 1000 
        //images).
        const int multiplesOfFifty = 1; //max is 20
        private string[] api_keys;
        private int default_key = 0;

        public BingScraper(string api_key)
        {
            this.api_key = api_key;
            this.root_url = "https://api.datamarket.azure.com/Bing/Search/";
            this.time_between_calls = 143;
            this.max_refinement = 5; //default value, should override in main method
            this.maxResultsPerQuery = 50;
            this.maxImagesPerSkip = maxResultsPerQuery;
            this.max_workerThreads = 10;
            this.max_refinement = 3;
        }

        public BingScraper(string[] api_keys) : this(api_keys[0])
        {
            this.api_keys = api_keys;
        }

        /// <summary>
        /// Usage: 
        ///     get_urls(brand, args[0]) to get the top maxResultsPerQuery * arg[0] results
        ///     get_urls(brand, args[0], args[1]) to get the top maxResultsPerQuery * arg[0] 
        ///         results, ignoring the top maxResultsPerQuery * args[1] results.
        /// </summary>
        /// <param name="brand"></param>
        /// <param name="args"></param>
        /// <returns></returns>
        public override HashSet<IQuery> get_urls(string brand, params object[] args)
        {
            //INVARIANT: brand is in lower case.

            //pattern obtained from example code from MSDN
            HashSet<IQuery> urls = new HashSet<IQuery>();
            var bingContainer = new BingSearchContainer(new Uri(root_url));

            //username can be anything, password is account key.
            bingContainer.Credentials = new NetworkCredential(api_key, api_key);
            //collect results from Bing Search API at fifty at a time.
            for (int i = (int) args[1]; i < (int) args[0] + (int) args[1]; i++)
            {

                var imageQuery = bingContainer.Image(brand, null, null, null, null, null, null);
                imageQuery.AddQueryOption("$top", maxResultsPerQuery); //number of results to get
                imageQuery.AddQueryOption("$skip", maxResultsPerQuery * i); //number of results to skip

                //write the query url
                Console.WriteLine("Querying: " + imageQuery.RequestUri);

                //try the next API key if the current query fails.
                ExecuteQuery:
                IEnumerable<ImageResult> imageResults;
                try
                {
                    imageResults = imageQuery.Execute();
                    Thread.Sleep(this.time_between_calls); //sleep so we don't piss the API off
                }
                catch
                {
                    api_key = api_keys[++default_key];
                    bingContainer.Credentials = new NetworkCredential(api_key, api_key);
                    goto ExecuteQuery;
                }

                //Store the URLs and print them.
                foreach (ImageResult result in imageResults)
                {
                    urls.Add(new BingQuery(result.MediaUrl, result.Title));
                    Console.WriteLine("Obtained URL: " + result.MediaUrl);
                }
            }
            return urls;
        }
  
    }

    public class BingQuery : AbstractQuery
    {
        public BingQuery(string url, string title)
        {
            this.url = url;
            this.title = title;
        }
    }
}
