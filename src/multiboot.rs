// crate/src/mutliboot.rs

/// Memory region type as defined by the Multiboot2 standard.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum MemoryRegionType {
    Available,
    Reserved,
    AcpiReclaimable,
    AcpiNvs,
    BadRam,
    Unknown(u32),
}

/// A single region of physical memory, already "translated" into a convenient form
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct MemoryRegion {
    pub base_addr: u64,
    pub lenght: u64,
    pub region_type: MemoryRegionType,
}

/// Possible parsing errors — an explicit enum is better
/// than a panic, because a malformed blob shouldn't crash
/// the kernel: we want to be able to handle it.
#[derive(Debug, PartialEq, Eq)]
pub enum ParseError {
    BufferTooSmall,
    InvalidEntrySize,
}

impl From<u32> for MemoryRegionType {
    fn from(value: u32) -> Self {
        match value {
            1 => MemoryRegionType::Available,
            2 => MemoryRegionType::Reserved,
            3 => MemoryRegionType::AcpiReclaimable,
            4 => MemoryRegionType::AcpiNvs,
            5 => MemoryRegionType::BadRam,
            other => MemoryRegionType::Unknown(other),
        }
    }
}

impl MemoryRegion {
    pub fn end_addr(&self) -> u64 {
        self.base_addr + self.lenght
    }
}

/// Extracts the list of memory regions from the "memory map" tag (type=6).
///
/// `tag_data` must be the byte slice starting IMMEDIATELY AFTER
/// the tag header (i.e., after type, size, entry_size, entry_version
/// read separately) and contains only the entry array.
pub fn parse_memory_map_entries(
    tag_data: &[u8],
    entry_size: usize,
) -> Result<impl Iterator<Item = MemoryRegion> + '_, ParseError> {
    if entry_size < 24 {
        return Err(ParseError::InvalidEntrySize);
    } else if tag_data.len() % entry_size != 0 {
        return Err(ParseError::BufferTooSmall);
    }

    Ok(tag_data.chunks_exact(entry_size).map(|chunk| {
        let base_addr = u64::from_le_bytes(chunk[0..8].try_into().unwrap());
        let length = u64::from_le_bytes(chunk[8..16].try_into().unwrap());
        let region_type_raw = u32::from_le_bytes(chunk[16..20].try_into().unwrap());

        MemoryRegion {
            base_addr,
            lenght,
            region_type: MemoryRegionType::from(region_type_raw),
        }
    }))
}
