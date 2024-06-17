namespace BaiduBce.Services.Bos.Model;

public class ListMultipartUploadsRequest : BucketRequestBase
{
	public string Prefix { get; set; }

	public string KeyMarker { get; set; }

	public string Delimiter { get; set; }

	public int? MaxUploads { get; set; }
}
