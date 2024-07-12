using BaiduBce.Model;
using System.Text.Json.Serialization;

namespace BaiduBce.Services.Sms.Model;

public class QuotaQueryResponse : BceResponseBase
{
	public int QuotaPerDay { get; set; }

	public int QuotaPerMonth { get; set; }

	public int QuotaRemainToday { get; set; }

	public int QuotaRemainThisMonth { get; set; }

	[JsonPropertyName("rateLimitPerMobilePerSignByDay")]
	public int RateLimitPerDay { get; set; }

	[JsonPropertyName("rateLimitPerMobilePerSignByHour")]
	public int RateLimitPerHour { get; set; }

	[JsonPropertyName("rateLimitPerMobilePerSignByMinute")]
	public int RateLimitPerMinute { get; set; }

	public bool RateLimitWhitelist { get; set; }

	[JsonPropertyName("applyQuotaPerDay")]
	public int QuotaPerDayApply { get; set; }

	[JsonPropertyName("applyQuotaPerMonth")]
	public int QuotaPerMonthApply { get; set; }

	[JsonPropertyName("applyCheckStatus")]
	public string QuotaApplyCheckStatus { get; set; }

	[JsonPropertyName("checkReply")]
	public string QuotaApplyCheckReply { get; set; }
}
