using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;
using System.IO;

namespace ReferenceImageSelector
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        const int sleepTime = 1000; //in milliseconds
        public static readonly TimeSpan MaxWait = Timeout.InfiniteTimeSpan;
        private AutoResetEvent keyStrokeReceived;
        private ImageSelect selector;
        private IEnumerator<ImageSource> images;

        public MainWindow()
        {
            keyStrokeReceived = new AutoResetEvent(false);
            selector = new ImageSelect();
            string wd = Directory.GetCurrentDirectory();
            InitializeComponent();
            this.KeyDown += ImageAccepted;
            images = selector.DisplayImages();
            images.Next();
            this.ImageFrame.Source = images.Current;
        }


        


        public void ImageAccepted(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter) 
            {
                selector.MoveImage(images.Next()); //need to store the Uri with the image?
                this.ImageFrame.Source = images.Current;
            }
            else if (e.Key == Key.X || e.Key == Key.Delete) 
            {
                images.Next();
                this.ImageFrame.Source = images.Current;
            }
            else
            {
                MessageBox.Show("Irrelevant key, will not advance! Press X or Delete to ignore \ncurrent image, press Enter to accept image.");
            }
        }
    }
}
