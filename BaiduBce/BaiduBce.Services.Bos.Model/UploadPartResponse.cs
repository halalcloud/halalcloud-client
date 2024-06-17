namespace BaiduBce.Services.Bos.Model;

public class UploadPartResponse : BosResponseBase
{
	public int PartNumber { get; set; }

	public string ETag { get; set; }
}
