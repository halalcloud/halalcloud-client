namespace BaiduBce.Services.Bos.Model;

public class ListPartsRequest : UploadRequestBase
{
	public int? MaxParts { get; set; }

	public int PartNumberMarker { get; set; }
}
