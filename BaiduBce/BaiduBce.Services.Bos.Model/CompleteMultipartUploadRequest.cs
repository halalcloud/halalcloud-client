using System.Collections.Generic;

namespace BaiduBce.Services.Bos.Model;

public class CompleteMultipartUploadRequest : UploadRequestBase
{
	public ObjectMetadata ObjectMetadata { get; set; }

	public List<PartETag> PartETags { get; set; }

	public CompleteMultipartUploadRequest()
	{
		ObjectMetadata = new ObjectMetadata();
	}
}
