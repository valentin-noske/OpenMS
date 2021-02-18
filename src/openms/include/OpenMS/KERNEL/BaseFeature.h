// --------------------------------------------------------------------------
//                   OpenMS -- Open-Source Mass Spectrometry
// --------------------------------------------------------------------------
// Copyright The OpenMS Team -- Eberhard Karls University Tuebingen,
// ETH Zurich, and Freie Universitaet Berlin 2002-2020.
//
// This software is released under a three-clause BSD license:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of any author or any participating institution
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// For a full list of authors, refer to the file AUTHORS.
// --------------------------------------------------------------------------
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Hendrik Weisser $
// $Authors: Hendrik Weisser, Chris Bielow $
// --------------------------------------------------------------------------

#pragma once

#include <OpenMS/KERNEL/RichPeak2D.h>
#include <OpenMS/METADATA/PeptideIdentification.h>
#include <OpenMS/METADATA/ID/IdentificationData.h>

#include <boost/optional.hpp>

namespace OpenMS
{
  class FeatureHandle;

  /**
    @brief A basic LC-MS feature.

    This class represents a "minimal" feature, defined by a position in RT and m/z, intensity,
    charge, quality, and annotated peptides. Most classes dealing with features will use the
    subclasses Feature or ConsensusFeature directly. However, algorithms that rely on very general
    characteristics of features can use this class to provide a unified solution for both "normal"
    features and consensus features.

    @ingroup Kernel
  */
  class OPENMS_DLLAPI BaseFeature : public RichPeak2D
  {
public:
    ///@name Type definitions
    //@{
    /// Type of quality values
    typedef float QualityType;
    /// Type of charge values
    typedef Int ChargeType;
    /// Type of feature width/FWHM (RT)
    typedef float WidthType;

    /// state of identification, use getAnnotationState() to query it
    enum AnnotationState
    {
      FEATURE_ID_NONE,
      FEATURE_ID_SINGLE,
      FEATURE_ID_MULTIPLE_SAME,
      FEATURE_ID_MULTIPLE_DIVERGENT,
      SIZE_OF_ANNOTATIONSTATE
    };

    static const std::string NamesOfAnnotationState[SIZE_OF_ANNOTATIONSTATE];

    //@}

    /** @name Constructors and Destructor
    */
    //@{
    /// Default constructor
    BaseFeature();

    /// Copy constructor
    BaseFeature(const BaseFeature& feature) = default;

    /// Move constructor
    BaseFeature(BaseFeature&& feature) = default;

    /// Copy constructor with a new map_index
    BaseFeature(const BaseFeature& rhs, UInt64 map_index);

    /// Constructor from raw data point
    explicit BaseFeature(const Peak2D& point);

    /// Constructor from raw data point with meta information
    explicit BaseFeature(const RichPeak2D& point);

    /// Constructor from a featurehandle
    explicit BaseFeature(const FeatureHandle& fh);

    /// Destructor
    ~BaseFeature() override;
    //@}

    /// @name Quality methods
    //@{
    /// Non-mutable access to the overall quality
    QualityType getQuality() const;
    /// Set the overall quality
    void setQuality(QualityType q);
    /// Compare by quality
    struct QualityLess :
      std::binary_function<BaseFeature, BaseFeature, bool>
    {
      bool operator()(const BaseFeature& left, const BaseFeature& right) const
      {
        return left.getQuality() < right.getQuality();
      }

      bool operator()(const BaseFeature& left, const QualityType& right) const
      {
        return left.getQuality() < right;
      }

      bool operator()(const QualityType& left, const BaseFeature& right) const
      {
        return left < right.getQuality();
      }

      bool operator()(const QualityType& left, const QualityType& right) const
      {
        return left < right;
      }

    };
    //@}

    /// Non-mutable access to the features width (full width at half max, FWHM)
    WidthType getWidth() const;
    /// Set the width of the feature (FWHM)
    void setWidth(WidthType fwhm);

    /// Non-mutable access to charge state
    const ChargeType& getCharge() const;

    /// Set charge state
    void setCharge(const ChargeType& ch);

    /// Assignment operator
    BaseFeature& operator=(const BaseFeature& rhs) = default;

    /// Move Assignment operator
    BaseFeature& operator=(BaseFeature&& rhs) & = default;

    /// Equality operator
    bool operator==(const BaseFeature& rhs) const;

    /// Inequality operator
    bool operator!=(const BaseFeature& rhs) const;

    /// returns a const reference to the PeptideIdentification vector
    const std::vector<PeptideIdentification>& getPeptideIdentifications() const;

    /// returns a mutable reference to the PeptideIdentification vector
    std::vector<PeptideIdentification>& getPeptideIdentifications();

    /// sets the PeptideIdentification vector
    void setPeptideIdentifications(const std::vector<PeptideIdentification>& peptides);

    /// state of peptide identifications attached to this feature. If one ID has multiple hits, the output depends on the top-hit only
    AnnotationState getAnnotationState() const;

    /// has a primary ID (peptide, RNA, compound) been assigned?
    bool hasPrimaryID() const;

    /**
       @brief Return the primary ID (peptide, RNA, compound) assigned to this feature.

       @throw Exception::MissingInformation if no ID was assigned
    */
    const IdentificationData::IdentifiedMolecule& getPrimaryID() const;

    /// clear any primary ID that was assigned
    void clearPrimaryID();

    /// set the primary ID (peptide, RNA, compound) for this feature
    void setPrimaryID(const IdentificationData::IdentifiedMolecule& id);

    /// immutable access to the set of matches (e.g. PSMs) with IDs for this feature
    const std::set<IdentificationData::ObservationMatchRef>& getIDMatches() const;

    /// mutable access to the set of matches (e.g. PSMs) with IDs for this feature
    std::set<IdentificationData::ObservationMatchRef>& getIDMatches();

    /// add an ID match (e.g. PSM) for this feature
    void addIDMatch(IdentificationData::ObservationMatchRef ref);

    /// update ID referenes (primary ID, matches) for this feature
    void updateIDReferences(const IdentificationData::RefTranslator& trans);

protected:

    /// Overall quality measure of the feature
    QualityType quality_;

    /// Charge of the peptide represented by this feature.  The default value is 0, which represents an unknown charge state.
    ChargeType charge_;

    /// Width (FWHM) for the feature. The default value is 0.0, a feature finding algorithm can compute this form the model.
    WidthType width_;

    /// PeptideIdentifications belonging to the feature
    std::vector<PeptideIdentification> peptides_;

    /// primary ID (peptide, RNA, compound) assigned to this feature
    boost::optional<IdentificationData::IdentifiedMolecule> primary_id_;

    /// set of observation matches (e.g. PSMs) with IDs for this feature
    std::set<IdentificationData::ObservationMatchRef> id_matches_;
  };

} // namespace OpenMS
