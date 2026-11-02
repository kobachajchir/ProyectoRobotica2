import React, { createContext, useContext, useEffect, useMemo, useState } from 'react';

export type ThemePreference = 'light' | 'dark' | 'system';
export type ResolvedTheme = 'light' | 'dark';

interface ThemeContextValue {
  preference: ThemePreference;
  resolvedTheme: ResolvedTheme;
  setPreference: (next: ThemePreference) => void;
}

const STORAGE_KEY = 'afr2-theme';

const ThemeContext = createContext<ThemeContextValue | null>(null);

function readInitialPreference(): ThemePreference {
  if (typeof window === 'undefined') return 'system';
  try {
    const raw = window.localStorage.getItem(STORAGE_KEY);
    if (raw === 'light' || raw === 'dark' || raw === 'system') {
      return raw;
    }
  } catch {
    // Ignorar almacenamiento bloqueado en modo privado
  }
  return 'system';
}

function getSystemDark(): boolean {
  if (typeof window === 'undefined' || typeof window.matchMedia !== 'function') {
    return true;
  }
  return window.matchMedia('(prefers-color-scheme: dark)').matches;
}

export function ThemeProvider({ children }: { children: React.ReactNode }) {
  const [preference, setPreferenceState] = useState<ThemePreference>(readInitialPreference);
  const [systemDark, setSystemDark] = useState<boolean>(getSystemDark);

  useEffect(() => {
    if (typeof window === 'undefined' || typeof window.matchMedia !== 'function') return;
    const media = window.matchMedia('(prefers-color-scheme: dark)');
    const handleChange = (event: MediaQueryListEvent) => {
      setSystemDark(event.matches);
    };
    setSystemDark(media.matches);
    media.addEventListener('change', handleChange);
    return () => media.removeEventListener('change', handleChange);
  }, []);

  const resolvedTheme: ResolvedTheme =
    preference === 'system' ? (systemDark ? 'dark' : 'light') : preference;

  useEffect(() => {
    if (typeof document === 'undefined') return;
    const root = document.documentElement;
    if (resolvedTheme === 'dark') {
      root.classList.add('dark');
    } else {
      root.classList.remove('dark');
    }
    // AFR2-20261101-07: el color de la barra del navegador sale del token --canvas del tema activo.
    const color = getComputedStyle(root).getPropertyValue('--canvas').trim();
    if (!color) return;
    const metaTags = document.querySelectorAll('meta[name="theme-color"]');
    metaTags.forEach((tag) => {
      tag.setAttribute('content', color);
    });
  }, [resolvedTheme]);

  // Rastrear modalidad de entrada (teclado = 0ms transiciones según §9.2)
  useEffect(() => {
    if (typeof document === 'undefined') return;
    const root = document.documentElement;
    const onKeyDown = (e: KeyboardEvent) => {
      if (e.key === 'Tab' || e.key.startsWith('Arrow') || e.key === 'Enter' || e.key === ' ') {
        root.dataset.input = 'keyboard';
      }
    };
    const onPointerDown = () => {
      root.dataset.input = 'pointer';
    };
    window.addEventListener('keydown', onKeyDown, { passive: true });
    window.addEventListener('pointerdown', onPointerDown, { passive: true });
    return () => {
      window.removeEventListener('keydown', onKeyDown);
      window.removeEventListener('pointerdown', onPointerDown);
    };
  }, []);

  const setPreference = (next: ThemePreference) => {
    setPreferenceState(next);
    try {
      window.localStorage.setItem(STORAGE_KEY, next);
    } catch {
      // Ignorar fallo de escritura
    }
  };

  const value = useMemo(
    () => ({ preference, resolvedTheme, setPreference }),
    [preference, resolvedTheme]
  );

  return <ThemeContext.Provider value={value}>{children}</ThemeContext.Provider>;
}

export function useTheme(): ThemeContextValue {
  const ctx = useContext(ThemeContext);
  if (!ctx) {
    throw new Error('useTheme debe usarse dentro de ThemeProvider');
  }
  return ctx;
}
