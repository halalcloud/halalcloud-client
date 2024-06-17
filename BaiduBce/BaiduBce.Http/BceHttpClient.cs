using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Reflection;
using BaiduBce.Auth;
using BaiduBce.Internal;
using BaiduBce.Util;
using log4net;

namespace BaiduBce.Http;

internal class BceHttpClient
{
	private static readonly ILog log = LogManager.GetLogger(typeof(BceHttpClient));

	public HttpWebResponse Execute(InternalRequest request)
	{
		BceClientConfiguration config = request.Config;
		if (config.Credentials != null)
		{
			if (config.Credentials is IBceSessionCredentials)
			{
				request.Headers["x-bce-security-token"] = (config.Credentials as IBceSessionCredentials).SessionToken;
			}
			request.Headers["Authorization"] = config.Signer.Sign(request);
		}
		HttpWebRequest httpWebRequest = CreateHttpWebRequest(request);
		PopulateRequestHeaders(request, httpWebRequest);
		if (request.Content != null)
		{
			if (request.Content.CanSeek)
			{
				request.Content.Position = request.StartPosition;
			}
			httpWebRequest.AllowWriteStreamBuffering = false;
			using Stream stream = WebRequestExtension.GetRequestStreamWithTimeout(httpWebRequest);
			byte[] array = new byte[config.SocketBufferSizeInBytes.Value];
			int num = 0;
			int num2 = 0;
			long contentLengthFromInternalRequest = GetContentLengthFromInternalRequest(request);
			try
			{
				while ((num = request.Content.Read(array, 0, array.Length)) > 0)
				{
					if (contentLengthFromInternalRequest > 0 && num + num2 >= contentLengthFromInternalRequest)
					{
						stream.Write(array, 0, (int)(contentLengthFromInternalRequest - num2));
						break;
					}
					stream.Write(array, 0, num);
					num2 += num;
				}
			}
			catch (NotSupportedException ex)
			{
				if (log.IsDebugEnabled)
				{
					log.Debug((object)"error when put data.", (Exception)ex);
				}
			}
		}
		try
		{
			return WebRequestExtension.GetResponseWithTimeout(httpWebRequest) as HttpWebResponse;
		}
		catch (WebException ex2)
		{
			if (ex2.Status != WebExceptionStatus.ProtocolError)
			{
				throw ex2;
			}
			using HttpWebResponse response = ex2.Response as HttpWebResponse;
			throw BceServiceException.CreateFromHttpWebResponse(response);
		}
	}

	private static HttpWebRequest CreateHttpWebRequest(InternalRequest request)
	{
		BceClientConfiguration config = request.Config;
		_ = config.Credentials;
		string text = request.Uri.AbsoluteUri;
		string canonicalQueryString = HttpUtils.GetCanonicalQueryString(request.Parameters, forSignature: false);
		if (canonicalQueryString.Length > 0)
		{
			text = text + "?" + canonicalQueryString;
		}
		HttpWebRequest httpWebRequest = WebRequest.Create(text) as HttpWebRequest;
		httpWebRequest.Timeout = config.TimeoutInMillis ?? 50000;
		httpWebRequest.ReadWriteTimeout = config.ReadWriteTimeoutInMillis ?? 50000;
		if (request.Range != null && request.Range.Length == 2)
		{
			AddRange(httpWebRequest, request.Range);
		}
		if (!string.IsNullOrEmpty(config.ProxyHost) && config.ProxyPort.GetValueOrDefault() > 0)
		{
			WebProxy webProxy = new WebProxy(config.ProxyHost, config.ProxyPort.GetValueOrDefault());
			if (config.ProxyCredentials != null)
			{
				webProxy.Credentials = config.ProxyCredentials;
			}
			httpWebRequest.Proxy = webProxy;
		}
		if (config.UseNagleAlgorithm.HasValue)
		{
			httpWebRequest.ServicePoint.UseNagleAlgorithm = config.UseNagleAlgorithm.Value;
		}
		httpWebRequest.ServicePoint.MaxIdleTime = config.MaxIdleTimeInMillis ?? BceClientConfiguration.DefaultMaxIdleTimeInMillis;
		httpWebRequest.ServicePoint.ConnectionLimit = config.ConnectionLimit ?? BceClientConfiguration.DefaultConnectionLimit;
		httpWebRequest.ServicePoint.Expect100Continue = request.Expect100Continue;
		httpWebRequest.Method = request.HttpMethod;
		return httpWebRequest;
	}

	private static void PopulateRequestHeaders(InternalRequest request, HttpWebRequest httpWebRequest)
	{
		httpWebRequest.UserAgent = request.Config.UserAgent;
		foreach (KeyValuePair<string, string> header in request.Headers)
		{
			string key = header.Key;
			if (key.Equals("Content-Length", StringComparison.CurrentCultureIgnoreCase))
			{
				httpWebRequest.ContentLength = Convert.ToInt64(header.Value);
			}
			else if (key.Equals("Content-Type", StringComparison.CurrentCultureIgnoreCase))
			{
				httpWebRequest.ContentType = header.Value;
			}
			else if (!key.Equals("Host", StringComparison.CurrentCultureIgnoreCase))
			{
				httpWebRequest.Headers[key] = header.Value;
			}
		}
	}

	private static long GetContentLengthFromInternalRequest(InternalRequest request)
	{
		if (request.Headers.TryGetValue("Content-Length", out var value) && long.TryParse(value, out var result))
		{
			return result;
		}
		return -1L;
	}

	private static void AddRange(HttpWebRequest httpWebRequest, long[] range)
	{
		Type typeFromHandle = typeof(HttpWebRequest);
		object[] parameters = new object[3]
		{
			"bytes",
			range[0].ToString(),
			range[1].ToString()
		};
		MethodInfo[] methods = typeFromHandle.GetMethods(BindingFlags.Instance | BindingFlags.NonPublic);
		foreach (MethodInfo methodInfo in methods)
		{
			if (methodInfo.Name == "AddRange")
			{
				methodInfo.Invoke(httpWebRequest, parameters);
			}
		}
	}
}
