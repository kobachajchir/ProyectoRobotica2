export interface ChassisSpinGraphicProps {
  direction: 'ccw' | 'cw';
  active?: boolean;
  size?: 'sm' | 'md';
  className?: string;
}

/**
 * Vista superior del chasis Mecanum de 4 ruedas con flecha circular antihoraria (`ccw`, izquierda)
 * u horaria (`cw`, derecha) alrededor del centro geometrico (AFR2-20261023-21 / VIS-018).
 * - Se emplea en los dos controles de giro sobre el eje de Control («Girar izquierda» y «Girar derecha»)
 *   y como simbolo reutilizable en los bloques de giro (`pivot`) del editor de secuencias.
 * - Representa la accion solicitada o programada; no anuncia giro fisico medido.
 */
export function ChassisSpinGraphic({
  direction,
  active = false,
  size = 'md',
  className = '',
}: ChassisSpinGraphicProps) {
  const isLeft = direction === 'ccw';
  const dim = size === 'sm' ? 'w-7 h-7' : 'w-11 h-11';

  return (
    <svg
      viewBox="0 0 64 64"
      fill="none"
      aria-hidden="true"
      className={`afr2-chassis-spin-graphic shrink-0 ${dim} ${className}`.trim()}
      data-direction={direction}
      data-active={active ? 'true' : 'false'}
    >
      {/* Arco circular de giro alrededor del eje vertical Z */}
      {isLeft ? (
        <>
          <path
            d="M 48 16 A 22 22 0 1 0 52 40"
            stroke="currentColor"
            strokeWidth="2.6"
            strokeLinecap="round"
            strokeDasharray="3 2"
            opacity={active ? 1 : 0.82}
          />
          {/* Punta de flecha antihoraria (arriba izquierda) */}
          <path
            d="M 22 9 L 13 15 L 23 19"
            stroke="currentColor"
            strokeWidth="2.8"
            strokeLinecap="round"
            strokeLinejoin="round"
          />
        </>
      ) : (
        <>
          <path
            d="M 16 16 A 22 22 0 1 1 12 40"
            stroke="currentColor"
            strokeWidth="2.6"
            strokeLinecap="round"
            strokeDasharray="3 2"
            opacity={active ? 1 : 0.82}
          />
          {/* Punta de flecha horaria (arriba derecha) */}
          <path
            d="M 42 9 L 51 15 L 41 19"
            stroke="currentColor"
            strokeWidth="2.8"
            strokeLinecap="round"
            strokeLinejoin="round"
          />
        </>
      )}

      {/* Ruedas Mecanum (FL, FR, RL, RR) con rodillos a 45° */}
      <g stroke="currentColor" strokeWidth="1.6">
        {/* FL */}
        <rect x="17" y="19" width="5.5" height="10" rx="1.4" fill="currentColor" fillOpacity="0.18" />
        <line x1="18" y1="21" x2="21.5" y2="27" strokeWidth="1.2" />
        {/* FR */}
        <rect x="41.5" y="19" width="5.5" height="10" rx="1.4" fill="currentColor" fillOpacity="0.18" />
        <line x1="46" y1="21" x2="42.5" y2="27" strokeWidth="1.2" />
        {/* RL */}
        <rect x="17" y="35" width="5.5" height="10" rx="1.4" fill="currentColor" fillOpacity="0.18" />
        <line x1="21.5" y1="37" x2="18" y2="43" strokeWidth="1.2" />
        {/* RR */}
        <rect x="41.5" y="35" width="5.5" height="10" rx="1.4" fill="currentColor" fillOpacity="0.18" />
        <line x1="42.5" y1="37" x2="46" y2="43" strokeWidth="1.2" />
      </g>

      {/* Cuerpo superior del chasis e indicador de frente */}
      <rect
        x="23.5"
        y="17"
        width="17"
        height="30"
        rx="3.5"
        stroke="currentColor"
        strokeWidth="2"
        fill="currentColor"
        fillOpacity={active ? '0.22' : '0.1'}
      />
      {/* Flecha frontal del chasis (nariz hacia arriba) */}
      <path
        d="M 28.5 23 L 32 19.2 L 35.5 23"
        stroke="currentColor"
        strokeWidth="1.8"
        strokeLinecap="round"
        strokeLinejoin="round"
      />
      {/* Eje central de rotacion */}
      <circle cx="32" cy="32" r="2.4" fill="currentColor" />
    </svg>
  );
}
