using System;
using BaiduBce.Model;

namespace BaiduBce.Services.Sts.Model;

public class GetSessionTokenResponse : BceResponseBase
{
	public string AccessKeyId { get; set; }

	public string SecretAccessKey { get; set; }

	public string SessionToken { get; set; }

	public DateTime Expiration { get; set; }
}
