using System;
using BaiduBce.Model;

namespace BaiduBce.Services.Sms.Model;

public class SignatureResponse : BceResponseBase
{
	public string SignatureId { get; set; }

	public string UserId { get; set; }

	public string Content { get; set; }

	public string ContentType { get; set; }

	public string Description { get; set; }

	public string Status { get; set; }

	public string CountryType { get; set; }

	public string Review { get; set; }

	public DateTime CreateDate { get; set; }

	public DateTime UpdateDate { get; set; }
}
