using System.IO;

namespace BaiduBce.Services.Bos.Model;

public class UploadPartRequest : UploadRequestBase
{
	public int PartNumber;

	public long PartSize;

	public string Md5Digest;

	public Stream InputStream;
}
