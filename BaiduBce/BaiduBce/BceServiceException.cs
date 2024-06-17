using System;
using System.IO;
using System.Net;
using BaiduBce.Util;

namespace BaiduBce;

public class BceServiceException : BceBaseException
{
	public class BceErrorResponse
	{
		public string RequestId { get; set; }

		public string Code { get; set; }

		public string Message { get; set; }
	}

	public override string Message => ErrorMessage + " (Status Code: " + StatusCode + "; Error Code: " + ErrorCode + "; Request ID: " + RequestId + ")";

	public string RequestId { get; set; }

	public string ErrorCode { get; set; }

	public string ErrorMessage { get; set; }

	public int StatusCode { get; set; }

	public BceServiceException()
	{
	}

	public BceServiceException(string message)
		: base(message)
	{
		ErrorMessage = message;
	}

	public BceServiceException(string message, Exception innerException)
		: base(message, innerException)
	{
		ErrorMessage = message;
	}

	public static BceServiceException CreateFromHttpWebResponse(HttpWebResponse response)
	{
		BceServiceException ex = null;
		Stream responseStream = response.GetResponseStream();
		if (responseStream != null)
		{
			BceErrorResponse bceErrorResponse = JsonUtils.ToObject<BceErrorResponse>(new StreamReader(responseStream));
			if (bceErrorResponse != null && bceErrorResponse.Message != null)
			{
				ex = new BceServiceException(bceErrorResponse.Message);
				ex.ErrorCode = bceErrorResponse.Code;
				ex.RequestId = bceErrorResponse.RequestId;
			}
		}
		if (ex == null)
		{
			ex = new BceServiceException(response.StatusDescription);
			ex.RequestId = response.Headers["x-bce-request-id"];
		}
		ex.StatusCode = (int)response.StatusCode;
		return ex;
	}
}
