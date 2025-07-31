DynamicCumulativeMarginPercent = 
VAR CurrentPercentile = MAX('PercentileAxis'[Value])

VAR RankedTable =
    ADDCOLUMNS(
        ALLSELECTED('YourTable'),
        "CustomerPercentile",
        ROUND(
            DIVIDE(
                RANKX(
                    ALLSELECTED('YourTable'),
                    CALCULATE(SUM('YourTable'[MARGIN_$])),
                    ,
                    DESC,
                    Dense
                ),
                COUNTROWS(ALLSELECTED('YourTable'))
            ) * 100,
            0
        )
    )

VAR CumulativeMargin =
    CALCULATE(
        SUMX(
            FILTER(RankedTable, [CustomerPercentile] <= CurrentPercentile),
            'YourTable'[MARGIN_$]
        )
    )

VAR TotalMargin =
    CALCULATE(SUM('YourTable'[MARGIN_$]), ALLSELECTED('YourTable'))

RETURN
    DIVIDE(CumulativeMargin, TotalMargin)