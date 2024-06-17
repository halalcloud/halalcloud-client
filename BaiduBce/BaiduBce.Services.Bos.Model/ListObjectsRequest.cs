namespace BaiduBce.Services.Bos.Model;

public class ListObjectsRequest : BucketRequestBase
{
	public string Prefix { get; set; }

	public string Marker { get; set; }

	public string Delimiter { get; set; }

	public int? MaxKeys { get; set; }
}
