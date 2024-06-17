namespace BaiduBce.Services.Bos.Model;

public class InitiateMultipartUploadResponse : BosResponseBase
{
	public string Key { get; set; }

	public string Bucket { get; set; }

	public string UploadId { get; set; }
}
