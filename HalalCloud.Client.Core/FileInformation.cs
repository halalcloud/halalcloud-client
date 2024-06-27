namespace HalalCloud.Client.Core
{
    public struct FileInformation
    {
        public DateTime CreationTime;
        public DateTime LastWriteTime;
        public ulong FileSize;
        public uint FileAttributes;
        public string FileName;

        public bool IsDirectory
        {
            get => 0 != (FileAttributes & Core.FileAttributes.Directory);
        }

        public bool IsHidden
        {
            get => 0 != (FileAttributes & Core.FileAttributes.Hidden);
        }
    }
}
