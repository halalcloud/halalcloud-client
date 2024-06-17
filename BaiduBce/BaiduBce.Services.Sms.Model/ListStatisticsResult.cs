namespace BaiduBce.Services.Sms.Model;

public class ListStatisticsResult
{
	public string Datetime { get; set; }

	public string CountryAlpha2Code { get; set; }

	public string SubmitCount { get; set; }

	public string SubmitLongCount { get; set; }

	public string ResponseSuccessCount { get; set; }

	public string ResponseSuccessProportion { get; set; }

	public string DeliverSuccessCount { get; set; }

	public string DeliverSuccessLongCount { get; set; }

	public string DeliverSuccessProportion { get; set; }

	public string DeliverFailureCount { get; set; }

	public string DeliverFailureProportion { get; set; }

	public string ReceiptProportion { get; set; }

	public string UnknownCount { get; set; }

	public string UnknownProportion { get; set; }

	public string ResponseTimeoutCount { get; set; }

	public string UnknownErrorCount { get; set; }

	public string NotExistCount { get; set; }

	public string SignatureOrTemplateCount { get; set; }

	public string AbnormalCount { get; set; }

	public string OverclockingCount { get; set; }

	public string OtherErrorCount { get; set; }

	public string BlacklistCount { get; set; }

	public string RouteErrorCount { get; set; }

	public string IssueFailureCount { get; set; }

	public string ParameterErrorCount { get; set; }

	public string IllegalWordCount { get; set; }

	public string AnomalyCount { get; set; }
}
