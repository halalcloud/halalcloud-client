using System;
using System.Collections.Generic;
using System.Net;
using BaiduBce.Internal;
using BaiduBce.Model;
using BaiduBce.Services.Sms.Model;
using BaiduBce.Util;

namespace BaiduBce.Services.Sms;

public class SmsClient : BceClientBase
{
	private const string UrlPrefix = "/sms/v3";

	private const string serviceEndpointFormat = "{0}://smsv3.{1}.baidubce.com";

	public SmsClient()
		: this(new BceClientConfiguration())
	{
	}

	public SmsClient(BceClientConfiguration config)
		: base(config, "{0}://smsv3.{1}.baidubce.com")
	{
	}

	private InternalRequest CreateSmsRequest(BceRequestBase request, string httpMethod, string[] pathComponents)
	{
		InternalRequest internalRequest = CreateInternalRequest(request, httpMethod, pathComponents);
		HashSet<string> headersToSign = new HashSet<string> { "x-bce-date", "Host" };
		internalRequest.Config.SignOptions.HeadersToSign = headersToSign;
		return internalRequest;
	}

	public SendMessageResponse SendMessage(SendMessageRequest request)
	{
		CheckNotNull(request, "request should NOT be null.");
		CheckNotNullOrEmpty(request.Mobile, "mobile is required.");
		CheckNotNullOrEmpty(request.SignatureId, "signatureId is required.");
		CheckNotNullOrEmpty(request.Template, "template is required.");
		InternalRequest internalRequest = CreateSmsRequest(request, "POST", new string[1] { "api/v3/sendsms" });
		if (!string.IsNullOrEmpty(request.ClientToken))
		{
			internalRequest.Parameters.Add("clientToken", request.ClientToken);
		}
		FillRequestBodyForJson(internalRequest, JsonUtils.SerializeObject(request));
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<SendMessageResponse>(httpWebResponse);
			}
		});
	}

	public TemplateCreateResponse CreateTemplate(TemplateCreateRequest request)
	{
		CheckNotNull(request, "object request should not be null");
		CheckNotNullOrEmpty(request.Content, "content should not be null or empty");
		CheckNotNullOrEmpty(request.CountryType, "countryType should not be null or empty");
		CheckNotNullOrEmpty(request.Name, "name should not be null or empty");
		CheckNotNullOrEmpty(request.SmsType, "smsType should not be null or empty");
		InternalRequest internalRequest = CreateSmsRequest(request, "POST", new string[2] { "/sms/v3", "/template" });
		FillRequestBodyForJson(internalRequest, JsonUtils.SerializeObject(request));
		internalRequest.Parameters.Add("clientToken", Guid.NewGuid().ToString());
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<TemplateCreateResponse>(httpWebResponse);
			}
		});
	}

	public void UpdateTemplate(TemplateUpdateRequest request)
	{
		CheckNotNull(request, "object request should not be null");
		CheckNotNullOrEmpty(request.TemplateId, "templateId should not be null or empty");
		InternalRequest internalRequest = CreateSmsRequest(request, "PUT", new string[3] { "/sms/v3", "/template", request.TemplateId });
		FillRequestBodyForJson(internalRequest, JsonUtils.SerializeObject(request));
		internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<BceResponseBase>(httpWebResponse);
			}
		});
	}

	public void DeleteTemplate(string templateId)
	{
		CheckNotNullOrEmpty(templateId, "templateId should not be null or empty");
		InternalRequest internalRequest = CreateSmsRequest(new BceRequestBase(), "DELETE", new string[3] { "/sms/v3", "/template", templateId });
		internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<BceResponseBase>(httpWebResponse);
			}
		});
	}

	public TemplateResponse GetTemplate(string templateId)
	{
		CheckNotNullOrEmpty(templateId, "templateId should not be null or empty");
		InternalRequest internalRequest = CreateSmsRequest(new BceRequestBase(), "GET", new string[3] { "/sms/v3", "/template", templateId });
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<TemplateResponse>(httpWebResponse);
			}
		});
	}

	public SignatureCreateResponse CreateSignature(SignatureCreateRequest request)
	{
		CheckNotNull(request, "object request should not be null");
		CheckNotNullOrEmpty(request.Content, "content should not be null or empty");
		CheckNotNullOrEmpty(request.ContentType, "contentType should not be null or empty");
		CheckNotNullOrEmpty(request.CountryType, "countryType should not be null or empty");
		InternalRequest internalRequest = CreateSmsRequest(request, "POST", new string[2] { "/sms/v3", "/signatureApply" });
		FillRequestBodyForJson(internalRequest, JsonUtils.SerializeObject(request));
		internalRequest.Parameters.Add("clientToken", Guid.NewGuid().ToString());
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<SignatureCreateResponse>(httpWebResponse);
			}
		});
	}

	public void UpdateSignature(SignatureUpdateRequest request)
	{
		CheckNotNull(request, "object request should not be null");
		CheckNotNullOrEmpty(request.SignatureId, "signatureId should not be null or empty");
		CheckNotNullOrEmpty(request.Content, "content should not be null or empty");
		CheckNotNullOrEmpty(request.ContentType, "contentType should not be null or empty");
		CheckNotNullOrEmpty(request.CountryType, "countryType should not be null or empty");
		InternalRequest internalRequest = CreateSmsRequest(request, "PUT", new string[3] { "/sms/v3", "/signatureApply", request.SignatureId });
		FillRequestBodyForJson(internalRequest, JsonUtils.SerializeObject(request));
		internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<BceResponseBase>(httpWebResponse);
			}
		});
	}

	public void DeleteSignature(string signatureId)
	{
		CheckNotNullOrEmpty(signatureId, "signatureId should not be null or empty");
		InternalRequest internalRequest = CreateSmsRequest(new BceRequestBase(), "DELETE", new string[3] { "/sms/v3", "/signatureApply", signatureId });
		internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<BceResponseBase>(httpWebResponse);
			}
		});
	}

	public SignatureResponse GetSignature(string signatureId)
	{
		CheckNotNullOrEmpty(signatureId, "signatureId should not be null or empty");
		InternalRequest internalRequest = CreateSmsRequest(new BceRequestBase(), "GET", new string[3] { "/sms/v3", "/signatureApply", signatureId });
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<SignatureResponse>(httpWebResponse);
			}
		});
	}

	public void UpdateQuotaRate(QuotaUpdateRequest request)
	{
		CheckNotNull(request, "object request should not be null");
		InternalRequest internalRequest = CreateSmsRequest(request, "PUT", new string[2] { "/sms/v3", "/quota" });
		FillRequestBodyForJson(internalRequest, JsonUtils.SerializeObject(request));
		internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<BceResponseBase>(httpWebResponse);
			}
		});
	}

	public QuotaQueryResponse QueryQuotaRate()
	{
		InternalRequest internalRequest = CreateSmsRequest(new BceRequestBase(), "GET", new string[2] { "/sms/v3", "/quota" });
		internalRequest.Parameters.Add("userQuery", "");
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<QuotaQueryResponse>(httpWebResponse);
			}
		});
	}

	public void CreateMobileBlack(BlackCreateRequest request)
	{
		CheckNotNull(request, "object request should not be null");
		CheckNotNullOrEmpty(request.Phone, "phone should not be null or empty");
		CheckNotNullOrEmpty(request.Type, "type should not be null or empty");
		if ("SignatureBlack".Equals(request.Type))
		{
			CheckNotNullOrEmpty(request.SignatureIdStr, "signatureIdStr should not be null or empty, when 'type' is 'SignatureBlack'.");
			CheckNotNullOrEmpty(request.SmsType, "smsType should not be null or empty, when 'type' is 'SignatureBlack'.");
		}
		InternalRequest internalRequest = CreateSmsRequest(request, "POST", new string[2] { "/sms/v3", "/blacklist" });
		FillRequestBodyForJson(internalRequest, JsonUtils.SerializeObject(request));
		internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<BceResponseBase>(httpWebResponse);
			}
		});
	}

	public void DeleteMobileBlack(string phones)
	{
		CheckNotNullOrEmpty(phones, "phones should not be null or empty");
		InternalRequest internalRequest = CreateSmsRequest(new BceRequestBase(), "DELETE", new string[2] { "/sms/v3", "/blacklist/delete" });
		internalRequest.Parameters.Add("phones", phones);
		internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<BceResponseBase>(httpWebResponse);
			}
		});
	}

	public BlackListResponse GetMobileBlack(BlackListRequest request)
	{
		CheckNotNull(request, "object should not be null");
		InternalRequest internalRequest = CreateSmsRequest(request, "GET", new string[2] { "/sms/v3", "/blacklist" });
		internalRequest.Parameters.Add("pageNo", request.PageNo.ToString());
		internalRequest.Parameters.Add("pageSize", request.PageSize.ToString());
		internalRequest.Parameters.Add("blackSource", "Console");
		if (!string.IsNullOrEmpty(request.Phone))
		{
			internalRequest.Parameters.Add("phone", request.Phone);
		}
		if (!string.IsNullOrEmpty(request.SmsType))
		{
			internalRequest.Parameters.Add("smsType", request.SmsType);
		}
		if (!string.IsNullOrEmpty(request.SignatureIdStr))
		{
			internalRequest.Parameters.Add("signatureId", request.SignatureIdStr);
		}
		if (!string.IsNullOrEmpty(request.StartTime))
		{
			internalRequest.Parameters.Add("startTime", request.StartTime);
		}
		if (!string.IsNullOrEmpty(request.EndTime))
		{
			internalRequest.Parameters.Add("endTime", request.EndTime);
		}
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<BlackListResponse>(httpWebResponse);
			}
		});
	}

	public StatisticsTemplateListResponse GetTemplateStatistics(StatisticsTemplateListRequest request)
	{
		CheckNotNull(request, "object should not be null");
		CheckNotNullOrEmpty(request.StartTime, "startTime should not be null or empty");
		CheckNotNullOrEmpty(request.EndTime, "endTime should not be null or empty");
		InternalRequest internalRequest = CreateSmsRequest(request, "GET", new string[2] { "/sms/v3", "/openapi/statisticTemplate" });
		internalRequest.Parameters.Add("startTime", request.StartTime);
		internalRequest.Parameters.Add("endTime", request.EndTime);
		internalRequest.Parameters.Add("pageNo", request.PageNo.ToString());
		internalRequest.Parameters.Add("pageSize", request.PageSize.ToString());
		if (!string.IsNullOrEmpty(request.TemplateId))
		{
			internalRequest.Parameters.Add("templateId", request.TemplateId);
		}
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<StatisticsTemplateListResponse>(httpWebResponse);
			}
		});
	}

	public ListStatisticsResponse GetListStatistics(ListStatisticsRequest request)
	{
		CheckNotNull(request, "statistics request should not be null");
		CheckNotNullOrEmpty(request.StartTime, "statistics request start time should not be null or empty");
		CheckNotNullOrEmpty(request.EndTime, "statistics request end time should not be null or empty");
		InternalRequest internalRequest = CreateSmsRequest(request, "GET", new string[2] { "/sms/v3", "/summary" });
		internalRequest.Parameters.Add("startTime", request.StartTime + " 00:00:00");
		internalRequest.Parameters.Add("endTime", request.EndTime + " 23:59:59");
		internalRequest.Parameters.Add("smsType", "all");
		internalRequest.Parameters.Add("dimension", "day");
		internalRequest.Parameters.Add("countryType", "domestic");
		if (!string.IsNullOrEmpty(request.SignatureId))
		{
			internalRequest.Parameters.Add("signatureId", request.SignatureId);
		}
		if (!string.IsNullOrEmpty(request.TemplateCode))
		{
			internalRequest.Parameters.Add("templateCode", request.TemplateCode);
		}
		if (!string.IsNullOrEmpty(request.SmsType))
		{
			internalRequest.Parameters["smsType"] = request.SmsType;
		}
		if (!string.IsNullOrEmpty(request.CountryType))
		{
			internalRequest.Parameters["countryType"] = request.CountryType;
		}
		return internalRequest.Config.RetryPolicy.Execute(delegate
		{
			HttpWebResponse httpWebResponse = httpClient.Execute(internalRequest);
			using (httpWebResponse)
			{
				return ToObject<ListStatisticsResponse>(httpWebResponse);
			}
		});
	}
}
