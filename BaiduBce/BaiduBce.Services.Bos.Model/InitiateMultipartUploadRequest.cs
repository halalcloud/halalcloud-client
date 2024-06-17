namespace BaiduBce.Services.Bos.Model;

public class InitiateMultipartUploadRequest : ObjectRequestBase
{
	public ObjectMetadata ObjectMetadata { get; set; }

	public InitiateMultipartUploadRequest()
	{
		ObjectMetadata = new ObjectMetadata();
	}
}
