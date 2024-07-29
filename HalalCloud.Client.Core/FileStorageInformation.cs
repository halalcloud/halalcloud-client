namespace HalalCloud.Client.Core
{
    public struct FileStorageInformation
    {
        public string Identifier;
        public long Size;
        public string Path;
        public FileStorageType Type;
        public List<FileStorageNode> Nodes;
    }
}
