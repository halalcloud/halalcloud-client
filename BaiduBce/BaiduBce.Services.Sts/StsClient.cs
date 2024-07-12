using System.Net;
using BaiduBce.Internal;
using BaiduBce.Services.Sts.Model;
using BaiduBce.Util;
using Microsoft.Extensions.Logging;

namespace BaiduBce.Services.Sts;

public class StsClient : BceClientBase
{
	private const string UrlPrefix = "/v1";

	private const string serviceEndpointFormat = "{0}://sts.{1}.baidubce.com";

    private static readonly ILogger Logger =
       LogUtils.Factory.CreateLogger<StsClient>();

    public StsClient()
		: this(new BceClientConfiguration
		{
			Protocol = "https"
		})
	{
	}

	public StsClient(BceClientConfiguration config)
		: base(config, "{0}://sts.{1}.baidubce.com")
	{
	}

	public GetSessionTokenResponse GetSessionToken()
	{
		return GetSessionToken(new GetSessionTokenRequest());
	}

	public GetSessionTokenResponse GetSessionToken(GetSessionTokenRequest request)
	{
		CheckNotNull(request, "request should not be null.");
		InternalRequest internalRequest = CreateInternalRequest(request, "POST", new string[2] { "/v1", "sessionToken" });
		if (request.DurationSeconds.HasValue)
		{
			internalRequest.Parameters["durationSeconds"] = request.DurationSeconds.ToString();
		}
		if (request.AccessControlList != null)
		{
			FillRequestBodyForJson(internalRequest, request.AccessControlList);
		}
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<GetSessionTokenResponse>(httpWebResponse);
			}
		});
	}
}
