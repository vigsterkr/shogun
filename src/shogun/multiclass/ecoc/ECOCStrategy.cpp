/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Chiyuan Zhang, Soeren Sonnenburg, Heiko Strathmann
 */

#include <shogun/multiclass/ecoc/ECOCStrategy.h>
#include <shogun/labels/BinaryLabels.h>
#include <shogun/labels/MulticlassLabels.h>

using namespace shogun;

CECOCStrategy::CECOCStrategy() : CMulticlassStrategy()
{
    init();
}

CECOCStrategy::CECOCStrategy(std::shared_ptr<CECOCEncoder >encoder, std::shared_ptr<CECOCDecoder >decoder)
	: CMulticlassStrategy()
{
    init();
    m_encoder=encoder;
    m_decoder=decoder;


}

void CECOCStrategy::init()
{
    m_encoder=NULL;
    m_decoder=NULL;

    SG_ADD(&m_encoder, "encoder", "ECOC Encoder");
    SG_ADD(&m_decoder, "decoder", "ECOC Decoder");
}

CECOCStrategy::~CECOCStrategy()
{


}

void CECOCStrategy::train_start(std::shared_ptr<CMulticlassLabels >orig_labels, std::shared_ptr<CBinaryLabels >train_labels)
{
    CMulticlassStrategy::train_start(orig_labels, train_labels);

    m_codebook = m_encoder->create_codebook(m_num_classes);
}

bool CECOCStrategy::train_has_more()
{
    return m_train_iter < m_codebook.num_rows;
}

SGVector<int32_t> CECOCStrategy::train_prepare_next()
{
    SGVector<int32_t> subset(m_orig_labels->get_num_labels(), false);
    int32_t tot=0;
    auto bl = binary_labels(m_train_labels);
    auto mc = multiclass_labels(m_orig_labels);
    for (int32_t i=0; i < m_orig_labels->get_num_labels(); ++i)
    {
        int32_t label = mc->get_int_label(i);
        switch (m_codebook(m_train_iter, label))
        {
        case -1:
            bl->set_label(i, -1);
            subset[tot++]=i;
            break;
        case 1:
            bl->set_label(i, 1);
            subset[tot++]=i;
            break;
        default:
            // 0 means ignore
            break;
        }
    }

    CMulticlassStrategy::train_prepare_next();
    return SGVector<int32_t>(subset.vector, tot, true);
}

int32_t CECOCStrategy::decide_label(SGVector<float64_t> outputs)
{
    return m_decoder->decide_label(outputs, m_codebook);
}

int32_t CECOCStrategy::get_num_machines()
{
    return m_codebook.num_cols;
}
