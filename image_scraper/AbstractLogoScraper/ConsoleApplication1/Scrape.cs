using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BingLogoScraper;
using FlickrLogoScraper;
using AbstractScraper;
using System.IO;
using ExtensionMethods;

namespace ConsoleApplication1
{
    public class Scrape
    {
        static readonly string[] bing_keys = {
                                        "a/n9IOt4XBNGa2+rKU+GHRUJinBDZBidNdNEATZqHyg", //michael zhao's key
                                        "kb2zSBeJHTEzk4istfO2V4hRUciVVLilerq4sSI6MWE",
                                        "hePDUbl1RM3Yo7CK9aOgp8DWkoUkKUzO+igm6u8/Zag",
                                        "YIsFyva6lCOJC0aMTxoHP3YXTGA09pmR+m0Pbphu6mg"
                                      };
        static readonly string[] new_bing_keys = {
                                        "rpOeeon5j5BwSeCOqO2HQ7xiUtfEYH/7Minb+U49e2I",
                                        "hgx2SUPlP781wYJKtdhTqOyA56l1x+MGMmzfWc22LN0",
                                        "Wu7SeOl241IqE5SLOYVVlR05mrV0GFQqo+In+znj0wY",
                                        "z4lFGNBdUapc0ABWh3+U1nt/IZMc0Kce9toh3cja+WI"
                                      };
        static readonly string[] flickr_keys = { 
                                                   "8dbd327f9680d26d20c872020435f5ca"
                                               };
        static readonly string[] flickr_secrets = {
                                                      "08e7e71586213119"
                                                  };

        const string actual_brand = "../../../../brand_names.txt";
        const string test_brand = "../../../../test_brand.txt";
        
        public static void Main(string[] args)
        {
            int refinements = 0;
            int workerThreads = 10;
            string brand = actual_brand;

            BingScraper bing = new BingScraper(new_bing_keys);
            bing.brand_file = brand;
            bing.max_refinement = refinements;
            bing.max_workerThreads = workerThreads;
            bing.run();
            //bing.download_list(bing.brand_list());


            int flickr_keyno = 0;
            FlickrScraper flickr = new FlickrScraper(flickr_keys[flickr_keyno], flickr_secrets[flickr_keyno]);
            flickr.brand_file = brand;
            flickr.max_refinement = refinements;
            flickr.max_workerThreads = workerThreads;
            flickr.download_list(flickr.brand_list(), true);

            Console.WriteLine("Let me finish downloading...");  
            Console.Read();
        }

    }
}
