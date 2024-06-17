using BaiduBce.Model;
using Newtonsoft.Json;

namespace BaiduBce.Services.Sms.Model;

public class QuotaQueryResponse : BceResponseBase
{
	public int QuotaPerDay { get; set; }

	public int QuotaPerMonth { get; set; }

	public int QuotaRemainToday { get; set; }

	public int QuotaRemainThisMonth { get; set; }

	[JsonProperty(PropertyName = "rateLimitPerMobilePerSignByDay")]
	public int RateLimitPerDay { get; set; }

	[JsonProperty(PropertyName = "rateLimitPerMobilePerSignByHour")]
	public int RateLimitPerHour { get; set; }

	[JsonProperty(PropertyName = "rateLimitPerMobilePerSignByMinute")]
	public int RateLimitPerMinute { get; set; }

	public bool RateLimitWhitelist { get; set; }

	[JsonProperty(PropertyName = "applyQuotaPerDay")]
	public int QuotaPerDayApply { get; set; }

	[JsonProperty(PropertyName = "applyQuotaPerMonth")]
	public int QuotaPerMonthApply { get; set; }

	[JsonProperty(PropertyName = "applyCheckStatus")]
	public string QuotaApplyCheckStatus { get; set; }

	[JsonProperty(PropertyName = "checkReply")]
	public string QuotaApplyCheckReply { get; set; }
}
