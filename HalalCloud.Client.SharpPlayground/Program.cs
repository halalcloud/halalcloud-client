using HalalCloud.Client.Core;
using System.Diagnostics;

namespace HalalCloud.Client.SharpPlayground
{
    internal class Program
    {
        static void Main(string[] args)
        {
            SessionManager Session = new SessionManager();

            //Session.Impersonate(
            //    @"rt__1fc4496396704771945da06329c329ea_c4b3676b-3339-4c8f-8300-6f24423a79ce");

            //Session.Logout();

            {
                Session.Authenticate(
                    (AuthenticationUri) =>
                    {
                        Console.WriteLine(AuthenticationUri);
                        Process.Start(new ProcessStartInfo
                        {
                            FileName = AuthenticationUri,
                            UseShellExecute = true
                        });

                        Console.WriteLine("Waiting...");
                    });

                if (Session.AccessToken != null)
                {
                    Console.WriteLine(
                        "AccessToken = {0}",
                        Session.AccessToken.AccessToken);
                    Console.WriteLine(
                        "RefreshToken = {0}",
                        Session.AccessToken.RefreshToken);
                }
            }

            //Session.CreateDirectory("/Folder2");

            //Session.UploadFile(
            //    @"C:\Users\mouri\Downloads\RISC-V Manual\unpriv-isa-asciidoc.html",
            //    "/unpriv-isa-asciidoc.html");

            List<FileInformation> Files = Session.EnumerateFiles("/");

            foreach (var Item in Files)
            {
                Console.WriteLine(
                    "{0}\t{1}\t{2}\t{3}",
                    Item.CreationTime,
                    Item.IsDirectory ? "<DIR>" : string.Empty,
                    0 != Item.FileSize ? Item.FileSize.ToString() : string.Empty,
                    Item.FileName);
            }

            Console.WriteLine("Hello, World!");
            Console.ReadKey();
        }
    }
}
