namespace HalalCloud.Client.Core
{
    public class Global
    {
        private static readonly string CachedRootPath = Path.Combine(
             Environment.GetFolderPath(
                Environment.SpecialFolder.LocalApplicationData),
             "HalalCloud");

        public static string RootPath
        {
            get
            {
                Utilities.EnsurePathExists(CachedRootPath);
                return CachedRootPath;
            }
        }
    }
}
