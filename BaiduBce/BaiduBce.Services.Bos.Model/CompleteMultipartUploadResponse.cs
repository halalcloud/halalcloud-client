namespace BaiduBce.Services.Bos.Model;

public class CompleteMultipartUploadResponse : BosResponseBase
{
	public string BucketName { get; set; }

	public string Key { get; set; }

	public string Location { get; set; }

	public string ETag { get; set; }
}
