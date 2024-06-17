namespace BaiduBce;

public static class BceConstants
{
	public static class Region
	{
		public const string Beijing = "bj";
	}

	public static class Protocol
	{
		public const string Http = "http";

		public const int HttpDefaultPort = 80;

		public const string Https = "https";

		public const int HttpsDefaultPort = 443;
	}

	public static class HttpHeaders
	{
		public const string Authorization = "Authorization";

		public const string ContentDisposition = "Content-Disposition";

		public const string ContentEncoding = "Content-Encoding";

		public const string TransferEncoding = "Transfer-Encoding";

		public const string ContentLength = "Content-Length";

		public const string ContentMd5 = "Content-MD5";

		public const string ContentRange = "Content-Range";

		public const string ContentType = "Content-Type";

		public const string Date = "Date";

		public const string ETag = "ETag";

		public const string Expires = "Expires";

		public const string Host = "Host";

		public const string LastModified = "Last-Modified";

		public const string Range = "Range";

		public const string Server = "Server";

		public const string UserAgent = "User-Agent";

		public const string BcePrefix = "x-bce-";

		public const string BceAcl = "x-bce-acl";

		public const string BceContentSha256 = "x-bce-content-sha256";

		public const string BceCopyMetadataDirective = "x-bce-metadata-directive";

		public const string BceCopySource = "x-bce-copy-source";

		public const string BceCopySourceIfMatch = "x-bce-copy-source-if-match";

		public const string BceDate = "x-bce-date";

		public const string BceUserMetadataPrefix = "x-bce-meta-";

		public const string BceRequestId = "x-bce-request-id";

		public const string BceSecurityToken = "x-bce-security-token";

		public const string BceStorageClass = "x-bce-storage-class";

		public const string BosDebugId = "x-bce-debug-id";

		public const string BosTrafficLimit = "x-bce-traffic-limit";
	}

	public static class HttpMethod
	{
		public const string Get = "GET";

		public const string Post = "POST";

		public const string Put = "PUT";

		public const string Delete = "DELETE";

		public const string Head = "HEAD";
	}

	public static class ContentType
	{
		public const string Json = "application/json; charset=utf-8";
	}

	public static class HttpStatusCode
	{
		public const int Continue = 100;

		public const int SwitchingProtocol = 101;

		public const int Processing = 102;

		public const int Ok = 200;

		public const int Created = 201;

		public const int Accepted = 202;

		public const int NonAuthoritativeInformation = 203;

		public const int NoContent = 204;

		public const int ResetContent = 205;

		public const int PartialContent = 206;

		public const int MultiStatus = 207;

		public const int MutlipleChoices = 300;

		public const int MovedPermanently = 301;

		public const int MovedTemporarily = 302;

		public const int SeeOther = 303;

		public const int NotModified = 304;

		public const int UseProxy = 305;

		public const int TemporaryRedirect = 307;

		public const int BadRequest = 400;

		public const int Unauthorized = 401;

		public const int PaymentRequired = 402;

		public const int Forbidden = 403;

		public const int NotFound = 404;

		public const int MethodNotAllowed = 405;

		public const int NotAcceptable = 406;

		public const int ProxyAuthenticationRequired = 407;

		public const int RequestTimeout = 408;

		public const int Conflict = 409;

		public const int Gone = 410;

		public const int LengthRequired = 411;

		public const int PreconditionFailed = 412;

		public const int RequestEntityTooLarge = 413;

		public const int RequestUriTooLong = 414;

		public const int UnsupportedMediaType = 415;

		public const int RequestedRangeNotSatisfiable = 416;

		public const int ExpectationFailed = 417;

		public const int InsufficientSpaceOnResource = 419;

		public const int MethodFailure = 420;

		public const int UnprocessableEntity = 422;

		public const int Locked = 423;

		public const int FailedDependency = 424;

		public const int InternalServerError = 500;

		public const int NotImplemented = 501;

		public const int BadGateway = 502;

		public const int ServiceUnavailable = 503;

		public const int GatewayTimeout = 504;

		public const int HttpVersionNotSupported = 505;

		public const int InsufficientStorage = 507;
	}

	public static class BceErrorCode
	{
		public const string AccessDenied = "AccessDenied";

		public const string InappropriateJson = "InappropriateJSON";

		public const string InternalError = "InternalError";

		public const string InvalidAccessKeyId = "InvalidAccessKeyId";

		public const string InvalidHttpAuthHeader = "InvalidHTTPAuthHeader";

		public const string InvalidHttpRequest = "InvalidHTTPRequest";

		public const string InvalidUri = "InvalidURI";

		public const string MalformedJson = "MalformedJSON";

		public const string InvalidVersion = "InvalidVersion";

		public const string OptInRequired = "OptInRequired";

		public const string PreconditionFailed = "PreconditionFailed";

		public const string RequestExpired = "RequestExpired";

		public const string SignatureDoesNotMatch = "SignatureDoesNotMatch";

		public const string Http400 = "Http400";
	}
}
