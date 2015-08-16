using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using FlickrNet;
using System.Net;
using System.IO;
using AbstractScraper;
using ExtensionMethods;
using System.Collections.ObjectModel;

namespace FlickrLogoScraper
{
    public class FlickrScraper : AbstractLogoScraper
    {
        public string secret { get; set; }
        const int max_results = 500; //max is 500
        const int max_page = 10; //max is 10
        private Flickr flickr;

        public FlickrScraper(string api_key, string secret)
        {
            this.api_key = api_key;
            this.secret = secret;
            this.root_url = "https://api.flickr.com/services";

            flickr = new Flickr();
            flickr.ApiKey = this.api_key;
            flickr.ApiSecret = this.secret;

            this.max_refinement = 5;
            this.time_between_calls = 1000;
        }

        /// <summary>
        /// Given a list of brands, returns a map from brand to SearchData.
        /// </summary>
        /// <param name="brands"></param>
        /// <returns></returns>
        public override HashSet<IQuery> get_urls(string brand, params object[] args)
        {
            HashSet<IQuery> urls = new HashSet<IQuery>();
            for (int page = 1; page <= max_page; page++)
            {
                var options = new PhotoSearchOptions
                {
                    PerPage = max_results,
                    Page = page,
                    Text = string.Format("{0}", brand),
                    Tags = string.Format("{0}", brand),
                    ContentType = ContentTypeSearch.PhotosOnly,
                    SortOrder = PhotoSearchSortOrder.Relevance,
                };

                    
                PhotoCollection photos;
                try
                {
                    photos = flickr.PhotosSearch(options);
                }
                catch { continue; }
                Console.WriteLine("Querying Flickr for: " + brand);
                String url = null;

                if (photos.Count == 0)
                    break;

                foreach (Photo photo in photos)
                    if (photo.IsPublic)
                    {
                        string tags = photo.Tags.Count == 0 ? photo.Title : photo.Tags.toStr(); //concatenates all tags by commas.
                        if (photo.MediumUrl != null)
                        {
                            urls.Add(new FlickrQuery(photo.MediumUrl, tags));
                            url = photo.MediumUrl;
                        }
                        else if (photo.LargeUrl != null)
                        {
                            urls.Add(new FlickrQuery(photo.LargeUrl, tags));
                            url = photo.LargeUrl;
                        }
                        else if (photo.SmallUrl != null)
                        {
                            urls.Add(new FlickrQuery(photo.SmallUrl, tags));
                            url = photo.SmallUrl;
                        }

                        if (url != null)
                            Console.WriteLine("Obtained URL: " + url);
                    }
                Thread.Sleep(this.time_between_calls); //sleep so we don't piss off Flickr.
            }
            return urls;
        }

    }

    public class FlickrQuery : AbstractQuery
    {
        public FlickrQuery(string url, string title)
        {
            this.url = url;
            this.title = title;
        }


    }
}
