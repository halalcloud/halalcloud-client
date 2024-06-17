using System.IO;

namespace BaiduBce.Services.Bos.Model;

public class PutObjectRequest : ObjectRequestBase
{
	public FileInfo FileInfo { get; set; }

	public Stream Stream { get; set; }

	public ObjectMetadata ObjectMetadata { get; set; }

	public PutObjectRequest()
	{
		ObjectMetadata = new ObjectMetadata();
	}
}
