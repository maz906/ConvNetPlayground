using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AbstractScraper
{
    /// <summary>
    /// Interface for logo scraper which pulls images of logos of certain brands off the internet.
    /// </summary>
    public interface ILogoScraper
    {
        /// <summary>
        /// Every logo scraper should have a single API key. If more are used, that
        /// should be stored outside of the class.
        /// </summary>
        string api_key { get; set; }

        /// <summary>
        /// Every logo scraper should have a single root URL which is going to 
        /// be accessed.
        /// </summary>
        string root_url { get; set; }

        /// <summary>
        /// Each API typically has a requirement of n calls per second. This should be
        /// the amount of time, in milliseconds, to sleep so that the API is not violated.
        /// </summary>
        int time_between_calls { get; set; }

        /// <summary>
        /// Every logo scraper should have a way to retrieve brand names from some source.
        /// </summary>
        /// <returns></returns>
        HashSet<string> brand_list();
        
        /// <summary>
        /// Every logo scraper should have a download method for each brand. A list of
        /// URLs is needed along with the brand name so files can be saved to a brand folder
        /// </summary>
        /// <param name="brand"></param>
        /// <param name="urls"></param>
        void download_images(string brand, HashSet<string> urls);

        /// <summary>
        /// Every logo scraper should obtain a list of URLs of images of logos associated with a brand
        /// </summary>
        /// <param name="brand"></param>
        /// <returns></returns>
        HashSet<IQuery> get_urls(string brand, params object[] args);

        /// <summary>
        /// Each logo scraper should begin with a brand, then have a refinement method whereby
        /// a better search term is used to gather more images.
        /// </summary>
        /// <param name="brand"></param>
        /// <returns></returns>
        HashSet<string> refined_searchTerms(string brand, HashSet<IQuery> urls);

        /// <summary>
        /// Method to run the scraper.
        /// </summary>
        void run();

    }

    /// <summary>
    /// An ILogoScraper is meant to obtain IQueries from an API or website. 
    /// </summary>
    public interface IQuery
    {
        string title { get; set; }
        string url { get; set; }
    }

    
}
