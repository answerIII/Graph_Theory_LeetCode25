use serde::{Deserialize, Serialize};
use std::{fs, path::Path};

/// A **data structure** that can be support MesssagePack.
/// **`RMPSupport`** provides **`from_msgpack`** and **`to_msgpack`** implementations for many Rust primitive and standard library types.
pub trait RMPSupport: Serialize + for<'de> Deserialize<'de> + Sized {
    fn from_msgpack(path: impl AsRef<Path>) -> crate::Result<Self> {
        let bytes = fs::read(path)?;
        let data: Self = rmp_serde::from_slice(&bytes)?;
        Ok(data)
    }

    fn to_msgpack(&self, path: impl AsRef<Path>) -> crate::Result<()> {
        let bytes = rmp_serde::to_vec(self)?;
        fs::write(path, bytes)?;
        Ok(())
    }
}
