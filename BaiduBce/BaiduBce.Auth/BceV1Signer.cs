using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Web;
using BaiduBce.Internal;
using BaiduBce.Util;
using log4net;

namespace BaiduBce.Auth;

public class BceV1Signer : ISigner
{
	private static readonly ILog log;

	private const string BceAuthVersion = "bce-auth-v1";

	private const string DefaultEncoding = "UTF-8";

	private static readonly HashSet<string> defaultHeadersToSign;

	public static string[] HexTable;

	static BceV1Signer()
	{
		log = LogManager.GetLogger(typeof(BceV1Signer));
		defaultHeadersToSign = new HashSet<string>();
		HexTable = (from v in Enumerable.Range(0, 256)
			select v.ToString("x2")).ToArray();
		defaultHeadersToSign.Add("Host".ToLower());
		defaultHeadersToSign.Add("Content-Length".ToLower());
		defaultHeadersToSign.Add("Content-Type".ToLower());
		defaultHeadersToSign.Add("Content-MD5".ToLower());
	}

	public string Sign(InternalRequest request)
	{
		if (request == null)
		{
			throw new ArgumentNullException("request should NOT be null.");
		}
		IBceCredentials obj = (request.Config ?? throw new ArgumentNullException("request.Config should NOT be null.")).Credentials ?? throw new ArgumentNullException("request.Config.Credentials should NOT be null.");
		SignOptions signOptions = request.Config.SignOptions;
		if (signOptions == null)
		{
			signOptions = new SignOptions();
		}
		string accessKeyId = obj.AccessKeyId;
		string secretKey = obj.SecretKey;
		DateTime dateTime = signOptions.Timestamp;
		if (dateTime == DateTime.MinValue)
		{
			dateTime = DateTime.Now;
		}
		string text = "bce-auth-v1/" + accessKeyId + "/" + DateUtils.FormatAlternateIso8601Date(dateTime) + "/" + signOptions.ExpirationInSeconds;
		string signingKey = Sha256Hex(secretKey, text);
		string canonicalURIPath = GetCanonicalURIPath(HttpUtility.UrlDecode(request.Uri.AbsolutePath));
		string canonicalQueryString = HttpUtils.GetCanonicalQueryString(request.Parameters, forSignature: true);
		SortedDictionary<string, string> headersToSign = GetHeadersToSign(request.Headers, signOptions.HeadersToSign);
		string canonicalHeaders = GetCanonicalHeaders(headersToSign);
		string text2 = "";
		if (signOptions.HeadersToSign != null)
		{
			text2 = string.Join(";", headersToSign.Keys.ToArray()).Trim().ToLower();
		}
		string text3 = request.HttpMethod + "\n" + canonicalURIPath + "\n" + canonicalQueryString + "\n" + canonicalHeaders;
		string text4 = Sha256Hex(signingKey, text3);
		string text5 = text + "/" + text2 + "/" + text4;
		log.Debug((object)string.Format("CanonicalRequest:{0}\tAuthorization:{1}", text3.Replace("\n", "[\\n]"), text5));
		return text5;
	}

	private static string GetCanonicalURIPath(string path)
	{
		if (path == null)
		{
			return "/";
		}
		if (path.StartsWith("/"))
		{
			return HttpUtils.NormalizePath(path);
		}
		return "/" + HttpUtils.NormalizePath(path);
	}

	private static string GetCanonicalHeaders(SortedDictionary<string, string> headers)
	{
		if (headers.Count == 0)
		{
			return "";
		}
		List<string> list = new List<string>();
		foreach (KeyValuePair<string, string> header in headers)
		{
			string key = header.Key;
			if (key != null)
			{
				string text = header.Value;
				if (text == null)
				{
					text = "";
				}
				list.Add(HttpUtils.Normalize(key.Trim().ToLower()) + ":" + HttpUtils.Normalize(text.Trim()));
			}
		}
		list.Sort(string.CompareOrdinal);
		return string.Join("\n", list.ToArray());
	}

	private static SortedDictionary<string, string> GetHeadersToSign(IDictionary<string, string> headers, HashSet<string> headersToSign)
	{
		SortedDictionary<string, string> sortedDictionary = new SortedDictionary<string, string>();
		if (headersToSign != null)
		{
			HashSet<string> hashSet = new HashSet<string>();
			foreach (string item in headersToSign)
			{
				hashSet.Add(item.Trim().ToLower());
			}
			headersToSign = hashSet;
		}
		foreach (KeyValuePair<string, string> header in headers)
		{
			string key = header.Key;
			if (!string.IsNullOrEmpty(header.Value) && ((headersToSign == null && IsDefaultHeaderToSign(key)) || (headersToSign != null && headersToSign.Contains(key.ToLower()) && !"Authorization".Equals(key, StringComparison.OrdinalIgnoreCase))))
			{
				sortedDictionary.Add(key, header.Value);
			}
		}
		return sortedDictionary;
	}

	private static bool IsDefaultHeaderToSign(string header)
	{
		header = header.Trim().ToLower();
		if (!header.StartsWith("x-bce-"))
		{
			return defaultHeadersToSign.Contains(header);
		}
		return true;
	}

	private static string Sha256Hex(string signingKey, string stringToSign)
	{
		try
		{
			return EncodeHex(new HMACSHA256(Encoding.UTF8.GetBytes(signingKey)).ComputeHash(Encoding.UTF8.GetBytes(stringToSign)));
		}
		catch (Exception innerException)
		{
			throw new BceClientException("Fail to generate the signature", innerException);
		}
	}

	private static string EncodeHex(byte[] data)
	{
		StringBuilder stringBuilder = new StringBuilder();
		foreach (byte b in data)
		{
			stringBuilder.Append(HexTable[b]);
		}
		return stringBuilder.ToString();
	}
}
