using System;

namespace BaiduBce.Auth;

public class DefaultBceSessionCredentials : DefaultBceCredentials, IBceSessionCredentials, IBceCredentials
{
	public string SessionToken { get; private set; }

	public DefaultBceSessionCredentials(string accessKeyId, string secretKey, string sessionToken)
		: base(accessKeyId, secretKey)
	{
		if (string.IsNullOrEmpty(sessionToken))
		{
			throw new ArgumentNullException("sessionToken should NOT be null or empty.");
		}
		SessionToken = sessionToken;
	}
}
