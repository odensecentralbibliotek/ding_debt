<?php

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
    'title' => t('User nonpayable debt list'),
    'description' => t('The current nonpayable debts for a user'),
    'single' => TRUE,
    'content_types' => array('list'),
    'required context' => new ctools_context_required(t('User'), 'user'),
    'category' => t('User'),
);

function ding_debt_nonpayable_debts_content_type_render($subtype, $conf, $panel_args, $context) {
    $account = isset($context->data) ? $context->data : NULL;
    $block = new stdClass();
    $block->module = 'nonpayable_ding_debt';
    $block->delta = 'nonpayable_debts';
    $block->title = t('Mellemværende som er sendt til inddrivelse');

    $debts = ding_provider_invoke_page('debt', 'list', $account, true);
    foreach ($debts as $index => $debt) {
        if ($debt->payableByClient == true) {
            unset($debts[$index]);
        }
    }
    $table = ding_debt_debts_nonpayable_form($debts);
    $block->content  = "<div>".t("Da dette mellemværende er sendt til inddrivelse, bedes du betale på udsendte faktura(er) eller ved personlig henvendelse på <a target='_blank' href='/biblioteker'>biblioteket</a>.")."</div><br/>";
    $block->content .= render($table)."<br/><br/>";
    return $block;
}

/*
 * Create table for invoices that cant be paid.
 */

function ding_debt_debts_nonpayable_form($debts = array()) {
    if (sizeof($debts) != 0) {
        $rows = array();
        $header = array(
            array('data' => t('Fee date'), 'class' => 'feedate'),
            array('data' => t('Material name'), 'class' => 'feetitle'),
            array('data' => t('Amount'), 'class' => 'feeamount'),
        );
        $rows = array();
        $is_invoice = false;
        foreach ($debts as $index => $debt) {
            $row = array(
                array('data' => format_date(strtotime(check_plain($debt->date)), 'date_only'), 'class' => 'feedate'),
                array('data' => $debt->display_name, 'class' => 'feetitle'),
                array('data' => ( $is_invoice ) ? number_format(($debt->amount - $debt->amount_paid), 2, ',', ' ') . ' <span class="note">*</span>' : number_format(($debt->amount - $debt->amount_paid), 2, ',', ' '), 'class' => 'feeamount'),
            );
            $rows[] = $row;
        }
        $table = theme('table', array(
            'header' => $header,
            'rows' => $rows,
        ));

        return $table;
    } else {
        return array();
    }
}
