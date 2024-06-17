namespace BaiduBce.Services.Bos.Model;

public class CopyObjectRequest : ObjectRequestBase
{
	public string SourceBucketName { get; set; }

	public string SourceKey { get; set; }

	public ObjectMetadata NewObjectMetadata { get; set; }

	public string ETag { get; set; }
}
