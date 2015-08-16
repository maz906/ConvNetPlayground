using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AbstractScraper;
using AmazonProductAdvtApi;

namespace AmazonLogoScraper
{
    public class AmazonScraper : AbstractLogoScraper
    {
        public string secret { get; set; }
        private const string DESTINATION = "ecs.amazonaws.com";

        public AmazonScraper()
        {
            this.api_key = "AKIAJB22MXYA6FSASPCQ"; //this is access key ID
            this.secret = "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"; //this is secret key
        }
        

        public override HashSet<IQuery> get_urls(string brand, params object[] args)
        {
            SignedRequestHelper helper = new SignedRequestHelper(api_key, secret, DESTINATION);


            return null;
        }
    }
}
