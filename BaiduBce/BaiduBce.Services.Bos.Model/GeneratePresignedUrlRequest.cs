using System.Collections.Generic;

namespace BaiduBce.Services.Bos.Model;

public class GeneratePresignedUrlRequest : ObjectRequestBase
{
	public string ContentType;

	public string ContentMd5;

	public int ExpirationInSeconds;

	public IDictionary<string, string> RequestParameters = new Dictionary<string, string>();

	public IDictionary<string, string> RequestHeaders = new Dictionary<string, string>();

	public string Method { get; set; }
}
