#ifndef __l1t_EMTF_muon_candidate_h__
#define __l1t_EMTF_muon_candidate_h__

#include <map>

namespace l1t {
  class EMTFMuonCand {
  public:

    explicit EMTFMuonCand(uint64_t dataword); // Should this be more bits? - AWB 11.01.16

  EMTFMuonCand() :
    m_track_phi_gmt(0), m_track_bx(0), m_track_quality(0), m_track_eta(0), m_track_pt(0)
      {};
      
  EMTFMuonCand(int int_track_phi_gmt, int int_track_bx, int int_track_quality, int int_track_eta, int int_track_pt) :
    m_track_phi_gmt(int_track_phi_gmt), m_track_bx(int_track_bx), m_track_quality(int_track_quality), 
      m_track_eta(int_track_eta), m_track_pt(int_track_pt), m_dataword(0)
      {};

      virtual ~EMTFMuonCand() {};

    /// Set CSC quality as ??? - AWB 11.01.16
    void set_track_phi_gmt(int bits) { m_track_phi_gmt = bits; };
    void set_track_bx(int bits) { m_track_bx = bits; };
    void set_track_quality(int bits) { m_track_quality = bits; };
    void set_track_eta(int bits) { m_track_eta = bits; };
    void set_track_pt(int bits) { m_track_pt = bits; };
    
    /// Set the 64 bit word coming from HW directly
    void setDataword(uint64_t bits) { m_dataword = bits; };
    
    /// Get CSC quality (returned ???) - AWB 11.01.16
    const int track_phi_gmt() const { return m_track_phi_gmt; };
    const int track_bx() const { return m_track_bx; };
    const int track_quality() const { return m_track_quality; };
    const int track_eta() const { return m_track_eta; };
    const int track_pt() const { return m_track_pt; };
    
    /// Get 64 bit data word
    const uint64_t dataword() const { return m_dataword; };
    

  private:
    int m_track_phi_gmt;
    int m_track_bx;
    int m_track_quality;
    int m_track_eta;
    int m_track_pt;
    
    /// This is the 64 bit word as transmitted in HW
    uint64_t m_dataword;
    
  };

}

#endif /* define __l1t_EMTF_muon_candidate_h__ */
