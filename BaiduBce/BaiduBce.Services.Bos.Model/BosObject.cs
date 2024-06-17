using System.IO;

namespace BaiduBce.Services.Bos.Model;

public class BosObject
{
	public string BucketName { get; set; }

	public string Key { get; set; }

	public ObjectMetadata ObjectMetadata { get; set; }

	public Stream ObjectContent { get; set; }
}
